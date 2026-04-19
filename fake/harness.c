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

static uint32_t tick_count = 0;
static int has_throttle = 0;    // whether throttle is being driven
static int throttle_value = 0;  // current throttle value

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
           "pwm_duty=%u pwm_arr=%u pwm_duty_count=%u\n",
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
           (unsigned)fake_pwm_duty, (unsigned)fake_pwm_arr, (unsigned)fake_pwm_duty_count);
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

    // Simulate processDshot/setInput which normally runs from ISR
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
    else if (strcmp(key, "interval_timer") == 0) { _TIM2_inst.CNT = v; }
    else if (strcmp(key, "zc") == 0 && v == 1) {
        // Force a zero-crossing: call interruptRoutine or PeriodElapsedCallback
        interruptRoutine();
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
