// Deterministic reset of ALL firmware globals to compile-time init values.

#include <string.h>
#include "am32_test_externs.h"

// Additional globals not in am32_test_externs.h (only needed for reset, not tests)
extern uint16_t target_e_com_time_high;
extern uint16_t target_e_com_time_low;
extern uint8_t crsf_input_channel;
extern uint8_t crsf_output_PWM_channel;
extern uint16_t ADC_CCR;
extern uint16_t current_angle;
extern uint16_t desired_angle;
extern char maximum_throttle_change_ramp;
extern uint16_t velocity_count;
extern uint16_t velocity_count_threshold;
extern char lowkv;
extern uint16_t sin_mode_min_s_d;
extern char startup_boost;
extern uint16_t low_pin_count;
extern char fast_accel;
extern char fast_deccel;
extern uint16_t stall_protect_minimum_duty;
extern char low_kv_filter_level;
extern uint16_t ADC_smoothed_input;
extern char telemetry_done;
extern uint8_t changeover_step;
extern uint8_t advancedivisor;
extern uint16_t step_delay;
extern uint16_t gate_drive_offset;
extern uint16_t armed_count_threshold;
extern char output_timer_prescaler;
extern uint8_t high_calibration_counts;
extern uint16_t last_high_threshold;
extern uint8_t low_calibration_counts;
extern int shift_amount;
extern uint16_t processtime;
extern uint16_t halfpulsetime;

void resetFirmwareState(void)
{
    // --- eeprom ---
    memset(&eepromBuffer, 0, sizeof(eepromBuffer));

    // --- main.c ---
    drive_by_rpm = 0;
    MAXIMUM_RPM_SPEED_CONTROL = 10000;
    MINIMUM_RPM_SPEED_CONTROL = 1000;

    memset(&speedPid, 0, sizeof(speedPid));
    speedPid.Kp = 10; speedPid.Kd = 100;
    speedPid.integral_limit = 10000; speedPid.output_limit = 50000;

    memset(&currentPid, 0, sizeof(currentPid));
    currentPid.Kp = 400; currentPid.Kd = 1000;
    currentPid.integral_limit = 20000; currentPid.output_limit = 100000;

    memset(&stallPid, 0, sizeof(stallPid));
    stallPid.Kp = 1; stallPid.Kd = 50;
    stallPid.integral_limit = 10000; stallPid.output_limit = 50000;

    polling_mode_changeover = 0;
    ramp_divider = 0;
    max_ramp_startup = RAMP_SPEED_STARTUP;
    max_ramp_low_rpm = RAMP_SPEED_LOW_RPM;
    max_ramp_high_rpm = RAMP_SPEED_HIGH_RPM;
    send_esc_info_flag = 0;
    eeprom_address = EEPROM_START_ADD;
    prop_brake_duty_cycle = 0;
    ledcounter = 0;
    ramp_count = 0;
    process_time = 0;
    start_process = 0;
    one_khz_loop_counter = 0;
    target_e_com_time_high = 0;
    target_e_com_time_low = 0;
    compute_dshot_flag = 0;
    crsf_input_channel = 1;
    crsf_output_PWM_channel = 2;
    telemetry_interval_ms = 30;
    temp_advance = 0;
    motor_kv = 2000;
    dead_time_override = DEAD_TIME;
    stall_protect_target_interval = TARGET_STALL_PROTECTION_INTERVAL;
    enter_sine_angle = 180;
    do_once_sinemode = 0;
    auto_advance_level = 0;
    servo_low_threshold = 1100;
    servo_high_threshold = 1900;
    servo_neutral = 1500;
    servo_dead_band = 100;
    LOW_VOLTAGE_CUTOFF = 0;
    low_cell_volt_cutoff = 330;
    ADC_CCR = 30;
    current_angle = 90;
    desired_angle = 90;
    return_to_center = 0;
    target_e_com_time = 0;
    use_speed_control_loop = 0;
    input_override = 0;
    use_current_limit_adjust = 2000;
    use_current_limit = 0;
    stall_protection_adjust = 0;
    MCU_Id = 0;
    REV_Id = 0;
    reverse_speed_threshold = 1500;
    desync_happened = 0;
    maximum_throttle_change_ramp = 1;
    crawler_mode = 0;
    velocity_count = 0;
    velocity_count_threshold = 75;
    low_rpm_throttle_limit = 1;
    low_voltage_count = 0;
    telem_ms_count = 0;
    VOLTAGE_DIVIDER = TARGET_VOLTAGE_DIVIDER;
    cell_count = 0;
    brushed_direction_set = 0;
    tenkhzcounter = 0;
    consumed_current = 0;
    smoothed_raw_current = 0;
    actual_current = 0;
    lowkv = 0;
    min_startup_duty = 120;
    sin_mode_min_s_d = 120;
    bemf_timeout = 10;
    startup_boost = 50;
    reversing_dead_band = 1;
    low_pin_count = 0;
    max_duty_cycle_change = 2;
    fast_accel = 1;
    fast_deccel = 0;
    last_duty_cycle = 0;
    duty_cycle_setpoint = 0;
    play_tone_flag = 0;
    startup_max_duty_cycle = 200;
    minimum_duty_cycle = DEAD_TIME;
    stall_protect_minimum_duty = DEAD_TIME;
    desync_check = 0;
    low_kv_filter_level = 20;
    tim1_arr = TIM1_AUTORELOAD;
    TIMER1_MAX_ARR = TIM1_AUTORELOAD;
    duty_cycle_maximum = 2000;
    low_rpm_level = 20;
    high_rpm_level = 70;
    throttle_max_at_low_rpm = 400;
    throttle_max_at_high_rpm = 2000;
    memset(commutation_intervals, 0, sizeof(commutation_intervals));
    average_interval = 0;
    last_average_interval = 0;
    ADC_smoothed_input = 0;
    ADC_raw_current = 0;
    converted_degrees = 0;
    PROCESS_ADC_FLAG = 0;
    send_telemetry = 0;
    telemetry_done = 0;
    prop_brake_active = 0;
    dshot_telemetry = 0;
    last_dshot_command = 0;
    old_routine = 1;
    adjusted_input = 0;
    smoothedcurrent = 0;
    readIndex = 0;
    total = 0;
    memset(readings, 0, sizeof(readings));
    bemf_timeout_happened = 0;
    changeover_step = 5;
    filter_level = 5;
    running = 0;
    advance = 0;
    advancedivisor = 6;
    rising = 1;
    step_delay = 100;
    stepper_sine = 0;
    forward = 1;
    gate_drive_offset = DEAD_TIME;
    stuckcounter = 0;
    bad_count = 0;
    bad_count_threshold = CPU_FREQUENCY_MHZ / 24;
    armed_count_threshold = 1000;
    armed = 0;
    zero_input_count = 0;
    input = 0;
    newinput = 0;
    inputSet = 0;
    dshot = 0;
    servoPwm = 0;
    zero_crosses = 0;
    zcfound = 0;
    min_bemf_counts_up = TARGET_MIN_BEMF_COUNTS;
    min_bemf_counts_down = TARGET_MIN_BEMF_COUNTS;
    bemfcounter = 0;
    duty_cycle = 0;
    step = 1;
    commutation_interval = 12500;
    waitTime = 0;
    signaltimeout = 0;
    lastzctime = 0;
    thiszctime = 0;
    adjusted_duty_cycle = 0;
    k_erpm = 0;
    e_rpm = 0;
    e_com_time = 0;
    armed_timeout_count = 0;

    // --- signal.c ---
    max_servo_deviation = 250;
    servorawinput = 0;
    smallestnumber = 20000;
    enter_calibration_count = 0;
    calibration_required = 0;
    high_calibration_counts = 0;
    high_calibration_set = 0;
    last_high_threshold = 0;
    low_calibration_counts = 0;
    last_input = 0;
    output_timer_prescaler = 0;
    buffersize = 32;
    average_signal_pulse = 0;
    average_count = 0;
    average_packet_length = 0;
    dshot_frametime_high = 50000;
    dshot_frametime_low = 0;

    // --- dshot.c ---
    EDT_ARM_ENABLE = 0;
    EDT_ARMED = 0;
    shift_amount = 0;
    command_count = 0;
    last_command = 0;
    high_pin_count = 0;
    memset(gcr, 0, sizeof(gcr));
    dshot_extended_telemetry = 0;
    processtime = 0;
    halfpulsetime = 0;
    programming_mode = 0;
    position = 0;
    new_byte = 0;
    dshot_goodcounts = 0;
    dshot_badcounts = 0;
    dshot_full_number = 0;
    memset(dpulse, 0, sizeof(dpulse));
    gcrnumber = 0;
    send_EDT_init = 0;
    send_EDT_deinit = 0;

    // --- fake peripherals ---
    memset(&_TIM1_inst, 0, sizeof(_TIM1_inst));
    memset(&_TIM2_inst, 0, sizeof(_TIM2_inst));
    memset(&_TIM14_inst, 0, sizeof(_TIM14_inst));
    memset(&_TIM17_inst, 0, sizeof(_TIM17_inst));
    memset(&_DBGMCU_inst, 0, sizeof(_DBGMCU_inst));
    memset(&_ADC1_inst, 0, sizeof(_ADC1_inst));

    // --- shared buffers ---
    memset(dma_buffer, 0, sizeof(dma_buffer));

    // --- PWM output captures ---
    fake_pwm_duty = 0;
    fake_pwm_arr = 0;
    fake_pwm_prescaler = 0;
    fake_pwm_duty_count = 0;
}
