// Black-box test harness for AM32 firmware.
// Reads commands from stdin, ticks the firmware, prints state to stdout.
//
// Protocol:
//   Input lines (one per line):
//     config <key>=<value>        Set eeprom or state before running
//     tick [<key>=<value> ...]    Advance one tick with optional overrides
//     ticks <N> [<key>=<value>...]  Advance N ticks (bulk, only prints final state)
//     state                       Print current state without ticking
//     quit                        Exit
//
//   Tick key=value options:
//     throttle=<0-2047>           Set newinput (dshot-style throttle)
//     comp=<0|1>                  Comparator output level for this tick
//     zc=1                        Force a zero-crossing event this tick
//     interval_timer=<N>          Set INTERVAL_TIMER count
//
//   Output (after each tick/state command):
//     key=value pairs on one line, space-separated

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "main.h"
#include "am32_test_externs.h"
#include "test_reset.h"

// Mock control
static uint8_t mock_comp_level = 0;

// HAL mock implementations for the harness
void __enable_irq(void) {}
void __disable_irq(void) {}
void maskPhaseInterrupts(void) {}
void changeCompInput(void) {}
void enableCompInterrupts(void) {}
uint8_t getCompOutputLevel(void) { return mock_comp_level; }
void comStep(int s) { (void)s; }
void allOff(void) {}
void fullBrake(void) {}
void allpwm(void) {}
void proportionalBrake(void) {}

// Build a DShot frame in dma_buffer from an 11-bit throttle value.
// Encodes proper CRC. Uses normal (non-inverted) CRC.
static void build_dshot_frame(uint16_t value) {
    uint8_t bits[16] = {0};
    for (int i = 0; i < 11; i++)
        bits[i] = (value >> (10 - i)) & 1;
    bits[11] = 0; // no telemetry request

    uint8_t crc = ((bits[0] ^ bits[4] ^ bits[8]) << 3 |
                   (bits[1] ^ bits[5] ^ bits[9]) << 2 |
                   (bits[2] ^ bits[6] ^ bits[10]) << 1 |
                   (bits[3] ^ bits[7] ^ bits[11]));
    bits[12] = (crc >> 3) & 1;
    bits[13] = (crc >> 2) & 1;
    bits[14] = (crc >> 1) & 1;
    bits[15] = crc & 1;

    uint32_t base = 1000;
    for (int i = 0; i < 16; i++) {
        dma_buffer[i * 2] = base;
        dma_buffer[i * 2 + 1] = base + (bits[i] ? 22 : 10);
        base += 32;
    }
    dshot_frametime_high = 600;
    dshot_frametime_low = 400;
}

static uint32_t tick_count = 0;
static int has_throttle = 0;    // whether throttle is being driven
static int throttle_value = 0;  // current throttle value
static int do_transfer = 0;    // call transfercomplete() this tick

static void print_state(void) {
    printf("tick=%u armed=%d running=%d step=%d forward=%d "
           "duty_cycle=%u duty_cycle_setpoint=%u adjusted_duty_cycle=%u "
           "commutation_interval=%u average_interval=%u "
           "e_com_time=%d e_rpm=%u zero_crosses=%u "
           "input=%u adjusted_input=%u newinput=%u "
           "bemfcounter=%u zcfound=%d rising=%d "
           "old_routine=%d stepper_sine=%d "
           "signaltimeout=%u armed_timeout_count=%u "
           "battery_voltage=%u actual_current=%d degrees_celsius=%d "
           "last_duty_cycle=%u prop_brake_active=%d "
           "inputSet=%d dshot=%d servoPwm=%d "
           "pwm_duty=%u pwm_arr=%u pwm_duty_count=%u "
           "duty_cycle_maximum=%u filter_level=%u "
           "send_telemetry=%d send_esc_info_flag=%d\n",
           tick_count,
           (int)armed, (int)running, (int)step, (int)forward,
           (unsigned)duty_cycle, (unsigned)duty_cycle_setpoint,
           (unsigned)adjusted_duty_cycle,
           (unsigned)commutation_interval, (unsigned)average_interval,
           (int)e_com_time, (unsigned)e_rpm, (unsigned)zero_crosses,
           (unsigned)input, (unsigned)adjusted_input, (unsigned)newinput,
           (unsigned)bemfcounter, (int)zcfound, (int)rising,
           (int)old_routine, (int)stepper_sine,
           (unsigned)signaltimeout, (unsigned)armed_timeout_count,
           (unsigned)battery_voltage, (int)actual_current, (int)degrees_celsius,
           (unsigned)last_duty_cycle, (int)prop_brake_active,
           (int)inputSet, (int)dshot, (int)servoPwm,
           (unsigned)fake_pwm_duty, (unsigned)fake_pwm_arr, (unsigned)fake_pwm_duty_count,
           (unsigned)duty_cycle_maximum, (unsigned)filter_level,
           (int)send_telemetry, (int)send_esc_info_flag);
    fflush(stdout);
}

static void do_tick(void) {
    // Apply persistent throttle each tick (simulates ISR-driven input)
    if (has_throttle) {
        newinput = throttle_value;
        signaltimeout = 0;
    }

    // Advance interval timer
    _TIM2_inst.CNT++;

    // Simulate ISR-driven input processing
    if (do_transfer) {
        transfercomplete();
        do_transfer = 0;
    }
    setInput();
    tenKhzRoutine();
    main_loop();

    tick_count++;
}

static void apply_kv(const char *key, const char *val) {
    int v = atoi(val);

    // Tick-level overrides
    if (strcmp(key, "throttle") == 0) {
        if (v < 0) { has_throttle = 0; } // throttle=-1 disables throttle injection
        else { throttle_value = v; has_throttle = 1; EDT_ARMED = 1; }
    }
    else if (strcmp(key, "comp") == 0) { mock_comp_level = v; }
    else if (strcmp(key, "transfer") == 0) { do_transfer = v; }
    else if (strcmp(key, "dshot_frame") == 0) { build_dshot_frame((uint16_t)v); do_transfer = 1; }
    else if (strcmp(key, "interval_timer") == 0) { _TIM2_inst.CNT = v; }
    else if (strncmp(key, "dma_", 4) == 0) {
        // dma_<index>=<value> sets dma_buffer[index]
        int idx = atoi(key + 4);
        if (idx >= 0 && idx < 64) { dma_buffer[idx] = (uint32_t)v; }
    }
    else if (strcmp(key, "zc") == 0 && v == 1) {
        // Force a zero-crossing: interruptRoutine masks comparator and sets up
        // commutation timer. PeriodElapsedCallback fires when timer expires
        // and calls commutate(). We call both to simulate the full sequence.
        interruptRoutine();
        PeriodElapsedCallback();
    }
    // Config: eeprom fields
    else if (strcmp(key, "eeprom.bi_direction") == 0) { eepromBuffer.bi_direction = v; }
    else if (strcmp(key, "eeprom.dir_reversed") == 0) { eepromBuffer.dir_reversed = v; }
    else if (strcmp(key, "eeprom.motor_poles") == 0) { eepromBuffer.motor_poles = v; }
    else if (strcmp(key, "eeprom.stall_protection") == 0) { eepromBuffer.stall_protection = v; }
    else if (strcmp(key, "eeprom.use_sine_start") == 0) { eepromBuffer.use_sine_start = v; }
    else if (strcmp(key, "eeprom.comp_pwm") == 0) { eepromBuffer.comp_pwm = v; }
    else if (strcmp(key, "eeprom.variable_pwm") == 0) { eepromBuffer.variable_pwm = v; }
    else if (strcmp(key, "eeprom.brake_on_stop") == 0) { eepromBuffer.brake_on_stop = v; }
    else if (strcmp(key, "eeprom.rc_car_reverse") == 0) { eepromBuffer.rc_car_reverse = v; }
    else if (strcmp(key, "eeprom.startup_power") == 0) { eepromBuffer.startup_power = v; }
    else if (strcmp(key, "eeprom.input_type") == 0) { eepromBuffer.input_type = v; }
    else if (strcmp(key, "eeprom.stuck_rotor_protection") == 0) { eepromBuffer.stuck_rotor_protection = v; }
    else if (strcmp(key, "eeprom.sine_mode_changeover_thottle_level") == 0) { eepromBuffer.sine_mode_changeover_thottle_level = v; }
    else if (strcmp(key, "eeprom.drag_brake_strength") == 0) { eepromBuffer.drag_brake_strength = v; }
    else if (strcmp(key, "eeprom.beep_volume") == 0) { eepromBuffer.beep_volume = v; }
    else if (strcmp(key, "eeprom.low_voltage_cut_off") == 0) { eepromBuffer.low_voltage_cut_off = v; }
    else if (strcmp(key, "eeprom.current_P") == 0) { eepromBuffer.current_P = v; }
    else if (strcmp(key, "eeprom.current_I") == 0) { eepromBuffer.current_I = v; }
    else if (strcmp(key, "eeprom.current_D") == 0) { eepromBuffer.current_D = v; }
    else if (strcmp(key, "eeprom.eeprom_version") == 0) { eepromBuffer.eeprom_version = v; }
    else if (strcmp(key, "eeprom.motor_kv") == 0) { eepromBuffer.motor_kv = v; }
    else if (strcmp(key, "eeprom.motor_poles") == 0) { eepromBuffer.motor_poles = v; }
    else if (strcmp(key, "eeprom.advance_level") == 0) { eepromBuffer.advance_level = v; }
    else if (strcmp(key, "eeprom.max_ramp") == 0) { eepromBuffer.max_ramp = v; }
    // Direct state overrides
    else if (strcmp(key, "armed") == 0) { armed = v; }
    else if (strcmp(key, "running") == 0) { running = v; }
    else if (strcmp(key, "inputSet") == 0) { inputSet = v; }
    else if (strcmp(key, "dshot") == 0) { dshot = v; }
    else if (strcmp(key, "servoPwm") == 0) { servoPwm = v; }
    else if (strcmp(key, "forward") == 0) { forward = v; }
    else if (strcmp(key, "step") == 0) { step = v; }
    else if (strcmp(key, "old_routine") == 0) { old_routine = v; }
    else if (strcmp(key, "zero_crosses") == 0) { zero_crosses = v; }
    else if (strcmp(key, "commutation_interval") == 0) { commutation_interval = v; }
    else if (strcmp(key, "zero_input_count") == 0) { zero_input_count = v; }
    else if (strcmp(key, "EDT_ARMED") == 0) { EDT_ARMED = v; }
    else if (strcmp(key, "EDT_ARM_ENABLE") == 0) { EDT_ARM_ENABLE = v; }
    else if (strcmp(key, "dshot_telemetry") == 0) { dshot_telemetry = v; }
    else if (strcmp(key, "signaltimeout") == 0) { signaltimeout = v; }
    else if (strcmp(key, "cell_count") == 0) { cell_count = v; }
    else if (strcmp(key, "battery_voltage") == 0) { battery_voltage = v; }
    else if (strcmp(key, "process_adc") == 0) { PROCESS_ADC_FLAG = v; }
    else if (strcmp(key, "degrees_celsius") == 0) { degrees_celsius = v; }
    else if (strcmp(key, "actual_current") == 0) { actual_current = v; }
    else if (strcmp(key, "bemf_timeout_happened") == 0) { bemf_timeout_happened = v; }
    else if (strcmp(key, "bemf_timeout") == 0) { bemf_timeout = v; }
    else if (strcmp(key, "eeprom.limits.temperature") == 0) { eepromBuffer.limits.temperature = v; }
    else if (strcmp(key, "eeprom.limits.current") == 0) { eepromBuffer.limits.current = v; }
    else if (strcmp(key, "prop_brake_active") == 0) { prop_brake_active = v; }
    else if (strcmp(key, "duty_cycle") == 0) { duty_cycle = v; }
    else if (strcmp(key, "last_duty_cycle") == 0) { last_duty_cycle = v; }
    else if (strcmp(key, "low_voltage_count") == 0) { low_voltage_count = v; }
    else if (strcmp(key, "stepper_sine") == 0) { stepper_sine = v; }
    else if (strcmp(key, "send_esc_info_flag") == 0) { send_esc_info_flag = v; }
    else if (strcmp(key, "send_telemetry") == 0) { send_telemetry = v; }
    else if (strcmp(key, "out_put") == 0) { out_put = v; }
    else if (strcmp(key, "calibration_required") == 0) { calibration_required = v; }
    else if (strcmp(key, "high_calibration_set") == 0) { high_calibration_set = v; }
    else if (strcmp(key, "high_calibration_counts") == 0) { high_calibration_counts = v; }
    else if (strcmp(key, "low_calibration_counts") == 0) { low_calibration_counts = v; }
    else if (strcmp(key, "servo_high_threshold") == 0) { servo_high_threshold = v; }
    else if (strcmp(key, "servo_low_threshold") == 0) { servo_low_threshold = v; }
    else if (strcmp(key, "enter_calibration_count") == 0) { enter_calibration_count = v; }
    else if (strcmp(key, "last_input") == 0) { last_input = v; }
    else if (strcmp(key, "adjusted_input") == 0) { adjusted_input = v; }
    else if (strcmp(key, "eeprom.telemetry_on_interval") == 0) { eepromBuffer.telemetry_on_interval = v; }
    else if (strcmp(key, "use_current_limit") == 0) { use_current_limit = v; }
    else if (strcmp(key, "use_speed_control_loop") == 0) { use_speed_control_loop = v; }
    else if (strcmp(key, "desync_check") == 0) { desync_check = v; }
    else if (strcmp(key, "average_interval") == 0) { average_interval = v; }
    else if (strcmp(key, "last_average_interval") == 0) { last_average_interval = v; }
    else {
        fprintf(stderr, "harness: unknown key '%s'\n", key);
    }
}

static void parse_kvs(char *args) {
    char *tok = strtok(args, " \t\n");
    while (tok) {
        char *eq = strchr(tok, '=');
        if (eq) {
            *eq = '\0';
            apply_kv(tok, eq + 1);
        }
        tok = strtok(NULL, " \t\n");
    }
}

int main(void) {
    char line[1024];

    resetFirmwareState();

    // Print ready marker
    printf("ready\n");
    fflush(stdout);

    while (fgets(line, sizeof(line), stdin)) {
        // Strip newline
        line[strcspn(line, "\n")] = 0;

        if (strncmp(line, "quit", 4) == 0) {
            break;
        }
        else if (strncmp(line, "reset", 5) == 0) {
            resetFirmwareState();
            tick_count = 0;
            mock_comp_level = 0;
            printf("reset\n");
            fflush(stdout);
        }
        else if (strncmp(line, "state", 5) == 0) {
            print_state();
        }
        else if (strncmp(line, "load_eeprom", 11) == 0) {
            loadEEpromSettings();
            printf("ok\n");
            fflush(stdout);
        }
        else if (strncmp(line, "config ", 7) == 0) {
            parse_kvs(line + 7);
            printf("ok\n");
            fflush(stdout);
        }
        else if (strncmp(line, "ticks ", 6) == 0) {
            char *rest = line + 6;
            char *space = strchr(rest, ' ');
            int n = atoi(rest);
            // Parse any key=value args after the count
            if (space) {
                parse_kvs(space + 1);
            }
            for (int i = 0; i < n; i++) {
                do_tick();
            }
            print_state();
        }
        else if (strncmp(line, "tick", 4) == 0) {
            if (line[4] == ' ') {
                parse_kvs(line + 5);
            }
            do_tick();
            print_state();
        }
        else {
            fprintf(stderr, "harness: unknown command '%s'\n", line);
        }
    }

    return 0;
}
