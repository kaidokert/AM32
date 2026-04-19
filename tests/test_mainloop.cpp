#include "am32_mocks.h"
#include <cstring>

extern "C" {
#include "am32_test_externs.h"
}

static void resetMainLoopState() {
    memset(&eepromBuffer, 0, sizeof(eepromBuffer));
    _TIM2_inst.CNT = 0;
    memset(commutation_intervals, 0, sizeof(commutation_intervals));
    e_com_time = 0;
    armed = 1;
    running = 1;
    input = 500;
    forward = 1;
    step = 1;
    old_routine = 0;
    zero_crosses = 200;
    commutation_interval = 1000;
    average_interval = 500;
    polling_mode_changeover = 2000;
    minimum_duty_cycle = 10;
    tim1_arr = 1999;
    TIMER1_MAX_ARR = 1999;
    prop_brake_active = 0;
    bemf_timeout_happened = 0;
    bemf_timeout = 10;
    stepper_sine = 0;
    tenkhzcounter = 0;
    consumed_current = 0;
    desync_check = 0;
    last_average_interval = 0;
    desync_happened = 0;
    last_duty_cycle = 500;
    min_startup_duty = 100;
    duty_cycle = 500;
    duty_cycle_setpoint = 500;
    duty_cycle_maximum = 2000;
    send_telemetry = 0;
    send_esc_info_flag = 0;
    PROCESS_ADC_FLAG = 0;
    ADC_raw_volts = 0;
    ADC_raw_current = 0;
    converted_degrees = 0;
    smoothed_raw_current = 0;
    low_voltage_count = 0;
    LOW_VOLTAGE_CUTOFF = 0;
    cell_count = 0;
    low_cell_volt_cutoff = 330;
    filter_level = 5;
    k_erpm = 0;
    low_rpm_throttle_limit = 0;
    auto_advance_level = 0;
    stuckcounter = 0;
    dshot = 0;
    servoPwm = 0;
    dshot_telemetry = 0;
    signaltimeout = 0;
    adjusted_input = 500;
    newinput = 500;
    VOLTAGE_DIVIDER = 110;
    actual_current = 0;
    battery_voltage = 0;
    degrees_celsius = 0;
    ADC_raw_temp = 0;
}

// ============================================================
// e_com_time calculation
// ============================================================

TEST_CASE("main_loop calculates e_com_time from commutation_intervals", "[mainloop]") {
    MockFixture f;
    ALLOW_CALL(f.mock, maskPhaseInterrupts());
    ALLOW_CALL(f.mock, getCompOutputLevel()).RETURN(0);

    resetMainLoopState();
    commutation_intervals[0] = 100;
    commutation_intervals[1] = 200;
    commutation_intervals[2] = 300;
    commutation_intervals[3] = 100;
    commutation_intervals[4] = 200;
    commutation_intervals[5] = 300;

    main_loop();

    // e_com_time = (100+200+300+100+200+300 + 4) >> 1 = 1204 >> 1 = 602
    REQUIRE(e_com_time == 602);
}

// ============================================================
// min_bemf_counts adjustment
// ============================================================

TEST_CASE("main_loop sets higher bemf counts during startup", "[mainloop]") {
    MockFixture f;
    ALLOW_CALL(f.mock, maskPhaseInterrupts());
    ALLOW_CALL(f.mock, getCompOutputLevel()).RETURN(0);

    resetMainLoopState();
    zero_crosses = 3; // < 5
    eepromBuffer.bi_direction = 0;

    main_loop();

    REQUIRE(min_bemf_counts_up == TARGET_MIN_BEMF_COUNTS * 2);
    REQUIRE(min_bemf_counts_down == TARGET_MIN_BEMF_COUNTS * 2);
}

TEST_CASE("main_loop sets normal bemf counts after startup", "[mainloop]") {
    MockFixture f;
    ALLOW_CALL(f.mock, maskPhaseInterrupts());
    ALLOW_CALL(f.mock, getCompOutputLevel()).RETURN(0);

    resetMainLoopState();
    zero_crosses = 100; // >= 5

    main_loop();

    REQUIRE(min_bemf_counts_up == TARGET_MIN_BEMF_COUNTS);
    REQUIRE(min_bemf_counts_down == TARGET_MIN_BEMF_COUNTS);
}

TEST_CASE("main_loop bidirectional startup uses +1 bemf counts", "[mainloop]") {
    MockFixture f;
    ALLOW_CALL(f.mock, maskPhaseInterrupts());
    ALLOW_CALL(f.mock, getCompOutputLevel()).RETURN(0);

    resetMainLoopState();
    zero_crosses = 3;
    eepromBuffer.bi_direction = 1;

    main_loop();

    REQUIRE(min_bemf_counts_up == TARGET_MIN_BEMF_COUNTS + 1);
}

// ============================================================
// Variable PWM
// ============================================================

TEST_CASE("main_loop variable_pwm mode 1 maps tim1_arr", "[mainloop]") {
    MockFixture f;
    ALLOW_CALL(f.mock, maskPhaseInterrupts());
    ALLOW_CALL(f.mock, getCompOutputLevel()).RETURN(0);

    resetMainLoopState();
    eepromBuffer.variable_pwm = 1;
    commutation_interval = 150; // between 96 and 200
    TIMER1_MAX_ARR = 1999;

    main_loop();

    REQUIRE(tim1_arr >= TIMER1_MAX_ARR / 2);
    REQUIRE(tim1_arr <= TIMER1_MAX_ARR);
}

// ============================================================
// Signal timeout
// ============================================================

TEST_CASE("main_loop signal timeout disarms when armed", "[mainloop]") {
    MockFixture f;
    ALLOW_CALL(f.mock, maskPhaseInterrupts());
    ALLOW_CALL(f.mock, getCompOutputLevel()).RETURN(0);

    resetMainLoopState();
    armed = 1;
    signaltimeout = (LOOP_FREQUENCY_HZ >> 1) + 1; // just over half-second threshold

    main_loop();

    REQUIRE(armed == 0);
    REQUIRE(input == 0);
}

// ============================================================
// Consumed current accumulation
// ============================================================

TEST_CASE("main_loop accumulates consumed current at 1s interval", "[mainloop]") {
    MockFixture f;
    ALLOW_CALL(f.mock, maskPhaseInterrupts());
    ALLOW_CALL(f.mock, getCompOutputLevel()).RETURN(0);

    resetMainLoopState();
    tenkhzcounter = LOOP_FREQUENCY_HZ + 1;
    actual_current = 1000;
    consumed_current = 0;

    main_loop();

    REQUIRE(consumed_current > 0);
    REQUIRE(tenkhzcounter == 0);
}

// ============================================================
// BEMF timeout clearing
// ============================================================

TEST_CASE("main_loop clears bemf timeout when zero_crosses high", "[mainloop]") {
    MockFixture f;
    ALLOW_CALL(f.mock, maskPhaseInterrupts());
    ALLOW_CALL(f.mock, getCompOutputLevel()).RETURN(0);

    resetMainLoopState();
    zero_crosses = 1001;
    bemf_timeout_happened = 50;

    main_loop();

    REQUIRE(bemf_timeout_happened == 0);
}

TEST_CASE("main_loop clears bemf timeout when adjusted_input zero", "[mainloop]") {
    MockFixture f;
    ALLOW_CALL(f.mock, maskPhaseInterrupts());
    ALLOW_CALL(f.mock, getCompOutputLevel()).RETURN(0);

    resetMainLoopState();
    adjusted_input = 0;
    bemf_timeout_happened = 50;

    main_loop();

    REQUIRE(bemf_timeout_happened == 0);
}

// ============================================================
// Desync detection
// ============================================================

TEST_CASE("main_loop detects desync on large interval change", "[mainloop]") {
    MockFixture f;
    ALLOW_CALL(f.mock, maskPhaseInterrupts());
    ALLOW_CALL(f.mock, getCompOutputLevel()).RETURN(0);

    resetMainLoopState();
    desync_check = 1;
    zero_crosses = 20;
    // e_com_time = 0, so average_interval = 0. Need nonzero.
    commutation_intervals[0] = 600;
    commutation_intervals[1] = 600;
    commutation_intervals[2] = 600;
    commutation_intervals[3] = 600;
    commutation_intervals[4] = 600;
    commutation_intervals[5] = 600;
    // e_com_time = (3600+4)>>1 = 1802, average_interval = 1802/3 = 600
    last_average_interval = 100; // big difference from 600
    input = 100;
    commutation_interval = 2000; // > 1000, so running=0

    main_loop();

    REQUIRE(desync_happened == 1);
    REQUIRE(zero_crosses == 0);
    REQUIRE(running == 0);
    REQUIRE(old_routine == 1);
}

// ============================================================
// Telemetry send
// ============================================================

// NOTE: telemetry send tests require USE_SERIAL_TELEMETRY defined in targets.h
// Without it, the send path is compiled out.

TEST_CASE("main_loop sends ESC info when flag set", "[mainloop]") {
    MockFixture f;
    ALLOW_CALL(f.mock, maskPhaseInterrupts());
    ALLOW_CALL(f.mock, getCompOutputLevel()).RETURN(0);

    resetMainLoopState();
    send_esc_info_flag = 1;

    main_loop();

    REQUIRE(send_esc_info_flag == 0);
}

// ============================================================
// ADC processing
// ============================================================

TEST_CASE("main_loop processes ADC when flag set", "[mainloop]") {
    MockFixture f;
    ALLOW_CALL(f.mock, maskPhaseInterrupts());
    ALLOW_CALL(f.mock, getCompOutputLevel()).RETURN(0);

    resetMainLoopState();
    PROCESS_ADC_FLAG = 1;
    ADC_raw_volts = 2048; // ~1.65V raw
    ADC_raw_current = 500;

    main_loop();

    REQUIRE(PROCESS_ADC_FLAG == 0);
    REQUIRE(battery_voltage > 0);
}

TEST_CASE("main_loop low voltage cutoff triggers", "[mainloop]") {
    MockFixture f;
    ALLOW_CALL(f.mock, maskPhaseInterrupts());
    ALLOW_CALL(f.mock, getCompOutputLevel()).RETURN(0);

    resetMainLoopState();
    PROCESS_ADC_FLAG = 1;
    eepromBuffer.low_voltage_cut_off = 1;
    cell_count = 3;
    low_cell_volt_cutoff = 330;
    battery_voltage = 900; // below 3*330=990
    low_voltage_count = 10000; // at threshold

    main_loop();

    REQUIRE(LOW_VOLTAGE_CUTOFF == 1);
    REQUIRE(armed == 0);
    REQUIRE(running == 0);
}

// ============================================================
// eRPM and duty_cycle_maximum
// ============================================================

TEST_CASE("main_loop calculates e_rpm", "[mainloop]") {
    MockFixture f;
    ALLOW_CALL(f.mock, maskPhaseInterrupts());
    ALLOW_CALL(f.mock, getCompOutputLevel()).RETURN(0);

    resetMainLoopState();
    running = 1;
    // Set commutation intervals so e_com_time = 1000
    for (int i = 0; i < 6; i++) commutation_intervals[i] = 333;
    // e_com_time = (1998+4)>>1 = 1001

    main_loop();

    // e_rpm = 1 * (600000 / 1001) = ~599
    REQUIRE(e_rpm > 0);
    REQUIRE(k_erpm == e_rpm / 10);
}

TEST_CASE("main_loop temperature limits duty_cycle_maximum", "[mainloop]") {
    MockFixture f;
    ALLOW_CALL(f.mock, maskPhaseInterrupts());
    ALLOW_CALL(f.mock, getCompOutputLevel()).RETURN(0);

    resetMainLoopState();
    running = 1;
    low_rpm_throttle_limit = 0;
    eepromBuffer.limits.temperature = 80;
    degrees_celsius = 85; // over limit
    for (int i = 0; i < 6; i++) commutation_intervals[i] = 333;

    main_loop();

    REQUIRE(duty_cycle_maximum < 2000);
}

// ============================================================
// Filter level
// ============================================================

TEST_CASE("main_loop sets high filter during startup", "[mainloop]") {
    MockFixture f;
    ALLOW_CALL(f.mock, maskPhaseInterrupts());
    ALLOW_CALL(f.mock, getCompOutputLevel()).RETURN(0);

    resetMainLoopState();
    zero_crosses = 50; // < 100
    commutation_interval = 600; // > 500
    for (int i = 0; i < 6; i++) commutation_intervals[i] = 333;

    main_loop();

    REQUIRE(filter_level == 12);
}

TEST_CASE("main_loop sets low filter at high speed", "[mainloop]") {
    MockFixture f;
    ALLOW_CALL(f.mock, maskPhaseInterrupts());
    ALLOW_CALL(f.mock, getCompOutputLevel()).RETURN(0);

    resetMainLoopState();
    zero_crosses = 200;
    commutation_interval = 30; // < 50
    for (int i = 0; i < 6; i++) commutation_intervals[i] = 20;

    main_loop();

    REQUIRE(filter_level == 2);
}
