#include "am32_mocks.h"
#include <cstring>

extern "C" {
#include "am32_test_externs.h"
}

static void resetTenKhzState() {
    memset(&eepromBuffer, 0, sizeof(eepromBuffer));
    _TIM2_inst.CNT = 0;
    duty_cycle = 0;
    duty_cycle_setpoint = 0;
    duty_cycle_maximum = 2000;
    use_current_limit_adjust = 2000;
    stall_protection_adjust = 0;
    stepper_sine = 0;
    input_override = 0;
    use_speed_control_loop = 0;
    armed = 1;
    running = 1;
    input = 500;
    forward = 1;
    step = 1;
    old_routine = 0;
    zero_crosses = 100;
    commutation_interval = 1000;
    average_interval = 500;
    polling_mode_changeover = 2000;
    minimum_duty_cycle = 10;
    last_duty_cycle = 500;
    adjusted_duty_cycle = 0;
    tim1_arr = 1999;
    max_duty_cycle_change = 2;
    ramp_divider = 0;
    ramp_count = 0;
    max_ramp_startup = 2;
    max_ramp_low_rpm = 6;
    max_ramp_high_rpm = 16;
    one_khz_loop_counter = 0;
    tenkhzcounter = 0;
    ledcounter = 0;
    prop_brake_active = 0;
    prop_brake_duty_cycle = 0;
    use_current_limit = 0;
    stall_protect_target_interval = 6500;
    bemfcounter = 0;
    zcfound = 0;
    min_bemf_counts_up = 2;
    min_bemf_counts_down = 2;
    rising = 1;
    bad_count = 0;
    bad_count_threshold = 2;
    dshot = 0;
    servoPwm = 0;
    send_telemetry = 0;
    telem_ms_count = 0;
    telemetry_interval_ms = 30;
    armed_timeout_count = 0;
    cell_count = 0;
    PROCESS_ADC_FLAG = 0;
    signaltimeout = 0;
}

// ============================================================
// Basic counter increments
// ============================================================

TEST_CASE("tenKhzRoutine increments counters", "[tenkhz]") {
    MockFixture f;
    ALLOW_CALL(f.mock, comStep(trompeloeil::_));
    ALLOW_CALL(f.mock, __disable_irq());
    ALLOW_CALL(f.mock, __enable_irq());
    ALLOW_CALL(f.mock, maskPhaseInterrupts());
    ALLOW_CALL(f.mock, getCompOutputLevel()).RETURN(0);
    ALLOW_CALL(f.mock, changeCompInput());
    ALLOW_CALL(f.mock, enableCompInterrupts());

    resetTenKhzState();

    tenKhzRoutine();

    REQUIRE(tenkhzcounter == 1);
    REQUIRE(ledcounter == 1);
    // ramp_count may reset if > ramp_divider
    REQUIRE(signaltimeout == 1);
}

TEST_CASE("tenKhzRoutine sets duty_cycle from setpoint", "[tenkhz]") {
    MockFixture f;
    ALLOW_CALL(f.mock, comStep(trompeloeil::_));
    ALLOW_CALL(f.mock, __disable_irq());
    ALLOW_CALL(f.mock, __enable_irq());
    ALLOW_CALL(f.mock, maskPhaseInterrupts());
    ALLOW_CALL(f.mock, getCompOutputLevel()).RETURN(0);
    ALLOW_CALL(f.mock, changeCompInput());
    ALLOW_CALL(f.mock, enableCompInterrupts());

    resetTenKhzState();
    duty_cycle_setpoint = 1000;
    last_duty_cycle = 1000; // match setpoint so ramp doesn't change it
    ramp_divider = 9; // hold duty (ramp_count 1 not > 9)

    tenKhzRoutine();

    REQUIRE(duty_cycle == 1000);
}

// ============================================================
// Arming sequence
// ============================================================

TEST_CASE("tenKhzRoutine arms after timeout with zero input", "[tenkhz]") {
    MockFixture f;
    ALLOW_CALL(f.mock, comStep(trompeloeil::_));
    ALLOW_CALL(f.mock, __disable_irq());
    ALLOW_CALL(f.mock, __enable_irq());
    ALLOW_CALL(f.mock, maskPhaseInterrupts());
    ALLOW_CALL(f.mock, getCompOutputLevel()).RETURN(0);
    ALLOW_CALL(f.mock, changeCompInput());
    ALLOW_CALL(f.mock, enableCompInterrupts());

    resetTenKhzState();
    armed = 0;
    running = 0;
    input = 0;
    inputSet = 1;
    adjusted_input = 0;
    armed_timeout_count = LOOP_FREQUENCY_HZ; // at threshold
    zero_input_count = 31; // > 30

    tenKhzRoutine();

    REQUIRE(armed == 1);
}

TEST_CASE("tenKhzRoutine does not arm if adjusted_input nonzero", "[tenkhz]") {
    MockFixture f;
    ALLOW_CALL(f.mock, comStep(trompeloeil::_));
    ALLOW_CALL(f.mock, __disable_irq());
    ALLOW_CALL(f.mock, __enable_irq());
    ALLOW_CALL(f.mock, maskPhaseInterrupts());
    ALLOW_CALL(f.mock, getCompOutputLevel()).RETURN(0);
    ALLOW_CALL(f.mock, changeCompInput());
    ALLOW_CALL(f.mock, enableCompInterrupts());

    resetTenKhzState();
    armed = 0;
    running = 0;
    input = 0;
    inputSet = 1;
    adjusted_input = 100; // nonzero resets timeout
    armed_timeout_count = 50000;

    tenKhzRoutine();

    REQUIRE(armed == 0);
    REQUIRE(armed_timeout_count == 0);
}

// ============================================================
// Telemetry interval
// ============================================================

TEST_CASE("tenKhzRoutine triggers telemetry on interval", "[tenkhz]") {
    MockFixture f;
    ALLOW_CALL(f.mock, comStep(trompeloeil::_));
    ALLOW_CALL(f.mock, __disable_irq());
    ALLOW_CALL(f.mock, __enable_irq());
    ALLOW_CALL(f.mock, maskPhaseInterrupts());
    ALLOW_CALL(f.mock, getCompOutputLevel()).RETURN(0);
    ALLOW_CALL(f.mock, changeCompInput());
    ALLOW_CALL(f.mock, enableCompInterrupts());

    resetTenKhzState();
    eepromBuffer.telemetry_on_interval = 1;
    telemetry_interval_ms = 30;
    // threshold = (30 - 1 + 1) * 20 = 600
    telem_ms_count = 600;

    tenKhzRoutine();

    REQUIRE(send_telemetry == 1);
    REQUIRE(telem_ms_count == 0);
}

// ============================================================
// Old routine BEMF polling
// ============================================================

TEST_CASE("tenKhzRoutine old routine triggers zcfoundroutine on bemf", "[tenkhz]") {
    MockFixture f;
    ALLOW_CALL(f.mock, comStep(trompeloeil::_));
    ALLOW_CALL(f.mock, __disable_irq());
    ALLOW_CALL(f.mock, __enable_irq());
    ALLOW_CALL(f.mock, changeCompInput());
    ALLOW_CALL(f.mock, enableCompInterrupts());
    // maskPhaseInterrupts called by old_routine path AND by zcfoundroutine->commutate...
    ALLOW_CALL(f.mock, maskPhaseInterrupts());
    // getCompOutputLevel: return 1 so !1=0=current_state, rising=1 means bad_count++
    // Actually we want bemfcounter to exceed threshold, so return 0 -> !0=1, rising=1 -> bemfcounter++
    ALLOW_CALL(f.mock, getCompOutputLevel()).RETURN(0);

    resetTenKhzState();
    old_routine = 1;
    running = 1;
    zcfound = 0;
    rising = 1;
    // getBemfState is called first and increments bemfcounter if polarity matches.
    // getCompOutputLevel returns 0 -> !0=1=current_state, rising=1 -> bemfcounter++
    // So set bemfcounter to min_bemf_counts_up so after getBemfState increments it,
    // it exceeds the threshold.
    bemfcounter = min_bemf_counts_up + 1; // after getBemfState increments, will exceed threshold
    zero_crosses = 2; // < 5 so zcfoundroutine while-loop breaks

    uint32_t zc_before = zero_crosses;
    tenKhzRoutine();

    // zcfoundroutine should have been called, incrementing zero_crosses
    // (zcfound gets set to 1 then reset to 0 by commutate inside zcfoundroutine)
    REQUIRE(zero_crosses == zc_before + 1);
}

// ============================================================
// 1kHz PID loops
// ============================================================

TEST_CASE("tenKhzRoutine current limit PID at 1kHz", "[tenkhz]") {
    MockFixture f;
    ALLOW_CALL(f.mock, comStep(trompeloeil::_));
    ALLOW_CALL(f.mock, __disable_irq());
    ALLOW_CALL(f.mock, __enable_irq());
    ALLOW_CALL(f.mock, maskPhaseInterrupts());
    ALLOW_CALL(f.mock, getCompOutputLevel()).RETURN(0);
    ALLOW_CALL(f.mock, changeCompInput());
    ALLOW_CALL(f.mock, enableCompInterrupts());

    resetTenKhzState();
    one_khz_loop_counter = PID_LOOP_DIVIDER + 1; // trigger 1kHz path
    use_current_limit = 1;
    running = 1;
    actual_current = 5000;
    eepromBuffer.limits.current = 10; // target = 10*2*100 = 2000
    memset(&currentPid, 0, sizeof(currentPid));
    currentPid.Kp = 100;
    currentPid.output_limit = 50000;
    currentPid.integral_limit = 10000;
    use_current_limit_adjust = 2000;

    tenKhzRoutine();

    REQUIRE(PROCESS_ADC_FLAG == 1);
    REQUIRE(one_khz_loop_counter == 0);
    // current (5000) > target (2000), PID output positive, adjust decreases
    REQUIRE(use_current_limit_adjust < 2000);
}

TEST_CASE("tenKhzRoutine stall protection PID", "[tenkhz]") {
    MockFixture f;
    ALLOW_CALL(f.mock, comStep(trompeloeil::_));
    ALLOW_CALL(f.mock, __disable_irq());
    ALLOW_CALL(f.mock, __enable_irq());
    ALLOW_CALL(f.mock, maskPhaseInterrupts());
    ALLOW_CALL(f.mock, getCompOutputLevel()).RETURN(0);
    ALLOW_CALL(f.mock, changeCompInput());
    ALLOW_CALL(f.mock, enableCompInterrupts());

    resetTenKhzState();
    one_khz_loop_counter = PID_LOOP_DIVIDER + 1;
    eepromBuffer.stall_protection = 1;
    running = 1;
    commutation_interval = 8000; // > stall_protect_target_interval (6500)
    memset(&stallPid, 0, sizeof(stallPid));
    stallPid.Kp = 1;
    stallPid.output_limit = 50000;
    stallPid.integral_limit = 10000;
    stall_protection_adjust = 0;

    tenKhzRoutine();

    // commutation_interval (8000) > target (6500), positive error -> adjust increases
    REQUIRE(stall_protection_adjust > 0);
}

// ============================================================
// Ramp rate limiting
// ============================================================

TEST_CASE("tenKhzRoutine ramp limits duty cycle increase", "[tenkhz]") {
    MockFixture f;
    ALLOW_CALL(f.mock, comStep(trompeloeil::_));
    ALLOW_CALL(f.mock, __disable_irq());
    ALLOW_CALL(f.mock, __enable_irq());
    ALLOW_CALL(f.mock, maskPhaseInterrupts());
    ALLOW_CALL(f.mock, getCompOutputLevel()).RETURN(0);
    ALLOW_CALL(f.mock, changeCompInput());
    ALLOW_CALL(f.mock, enableCompInterrupts());

    resetTenKhzState();
    ramp_divider = 0;
    ramp_count = 0; // will increment to 1, > ramp_divider(0)
    duty_cycle_setpoint = 1000;
    last_duty_cycle = 500;
    max_ramp_high_rpm = 16;
    average_interval = 200;
    zero_crosses = 200;

    tenKhzRoutine();

    // Duty should be ramped: 500 + 16 = 516 (not jump to 1000)
    REQUIRE(duty_cycle == 516);
    REQUIRE(last_duty_cycle == 516);
}

TEST_CASE("tenKhzRoutine ramp limits duty cycle decrease", "[tenkhz]") {
    MockFixture f;
    ALLOW_CALL(f.mock, comStep(trompeloeil::_));
    ALLOW_CALL(f.mock, __disable_irq());
    ALLOW_CALL(f.mock, __enable_irq());
    ALLOW_CALL(f.mock, maskPhaseInterrupts());
    ALLOW_CALL(f.mock, getCompOutputLevel()).RETURN(0);
    ALLOW_CALL(f.mock, changeCompInput());
    ALLOW_CALL(f.mock, enableCompInterrupts());

    resetTenKhzState();
    ramp_divider = 0;
    ramp_count = 0;
    duty_cycle_setpoint = 100;
    last_duty_cycle = 500;
    max_ramp_high_rpm = 16;
    average_interval = 200;
    zero_crosses = 200;

    tenKhzRoutine();

    // Duty should be ramped down: 500 - 16 = 484
    REQUIRE(duty_cycle == 484);
}

TEST_CASE("tenKhzRoutine holds duty when ramp_count below divider", "[tenkhz]") {
    MockFixture f;
    ALLOW_CALL(f.mock, comStep(trompeloeil::_));
    ALLOW_CALL(f.mock, __disable_irq());
    ALLOW_CALL(f.mock, __enable_irq());
    ALLOW_CALL(f.mock, maskPhaseInterrupts());
    ALLOW_CALL(f.mock, getCompOutputLevel()).RETURN(0);
    ALLOW_CALL(f.mock, changeCompInput());
    ALLOW_CALL(f.mock, enableCompInterrupts());

    resetTenKhzState();
    ramp_divider = 9;
    ramp_count = 0; // will increment to 1, not > 9
    duty_cycle_setpoint = 1000;
    last_duty_cycle = 500;

    tenKhzRoutine();

    // Should hold at last_duty_cycle
    REQUIRE(duty_cycle == 500);
}

// ============================================================
// Adjusted duty cycle calculation
// ============================================================

TEST_CASE("tenKhzRoutine calculates adjusted_duty_cycle when running", "[tenkhz]") {
    MockFixture f;
    ALLOW_CALL(f.mock, comStep(trompeloeil::_));
    ALLOW_CALL(f.mock, __disable_irq());
    ALLOW_CALL(f.mock, __enable_irq());
    ALLOW_CALL(f.mock, maskPhaseInterrupts());
    ALLOW_CALL(f.mock, getCompOutputLevel()).RETURN(0);
    ALLOW_CALL(f.mock, changeCompInput());
    ALLOW_CALL(f.mock, enableCompInterrupts());

    resetTenKhzState();
    ramp_divider = 9; // hold duty
    duty_cycle_setpoint = 1000;
    last_duty_cycle = 1000;
    armed = 1;
    running = 1;
    input = 500;
    tim1_arr = 1999;

    tenKhzRoutine();

    // adjusted_duty_cycle = (1000 * 1999 / 2000) + 1 = 1000
    REQUIRE(adjusted_duty_cycle == 1000);
}

TEST_CASE("tenKhzRoutine prop brake adjusts duty", "[tenkhz]") {
    MockFixture f;
    ALLOW_CALL(f.mock, comStep(trompeloeil::_));
    ALLOW_CALL(f.mock, __disable_irq());
    ALLOW_CALL(f.mock, __enable_irq());
    ALLOW_CALL(f.mock, maskPhaseInterrupts());
    ALLOW_CALL(f.mock, getCompOutputLevel()).RETURN(0);
    ALLOW_CALL(f.mock, changeCompInput());
    ALLOW_CALL(f.mock, enableCompInterrupts());

    resetTenKhzState();
    ramp_divider = 9;
    duty_cycle_setpoint = 0;
    last_duty_cycle = 0;
    armed = 1;
    running = 0;
    input = 0;
    prop_brake_active = 1;
    prop_brake_duty_cycle = 1000;
    tim1_arr = 1999;

    tenKhzRoutine();

    // adjusted_duty_cycle = tim1_arr - (1000 * 1999 / 2000) = 1999 - 999 = 1000
    REQUIRE(adjusted_duty_cycle == 1000);
}

// ============================================================
// Startup ramp selection
// ============================================================

TEST_CASE("tenKhzRoutine selects startup ramp rate during startup", "[tenkhz]") {
    MockFixture f;
    ALLOW_CALL(f.mock, comStep(trompeloeil::_));
    ALLOW_CALL(f.mock, __disable_irq());
    ALLOW_CALL(f.mock, __enable_irq());
    ALLOW_CALL(f.mock, maskPhaseInterrupts());
    ALLOW_CALL(f.mock, getCompOutputLevel()).RETURN(0);
    ALLOW_CALL(f.mock, changeCompInput());
    ALLOW_CALL(f.mock, enableCompInterrupts());

    resetTenKhzState();
    ramp_divider = 0;
    ramp_count = 0;
    zero_crosses = 50; // < 150
    last_duty_cycle = 100; // < 150
    duty_cycle_setpoint = 500;
    max_ramp_startup = 2;

    tenKhzRoutine();

    // Should use startup ramp: 100 + 2 = 102
    REQUIRE(duty_cycle == 102);
}

TEST_CASE("tenKhzRoutine selects high rpm ramp at speed", "[tenkhz]") {
    MockFixture f;
    ALLOW_CALL(f.mock, comStep(trompeloeil::_));
    ALLOW_CALL(f.mock, __disable_irq());
    ALLOW_CALL(f.mock, __enable_irq());
    ALLOW_CALL(f.mock, maskPhaseInterrupts());
    ALLOW_CALL(f.mock, getCompOutputLevel()).RETURN(0);
    ALLOW_CALL(f.mock, changeCompInput());
    ALLOW_CALL(f.mock, enableCompInterrupts());

    resetTenKhzState();
    ramp_divider = 0;
    ramp_count = 0;
    zero_crosses = 200;
    last_duty_cycle = 500;
    duty_cycle_setpoint = 1000;
    average_interval = 200; // <= 500 -> high rpm
    max_ramp_high_rpm = 16;

    tenKhzRoutine();

    REQUIRE(duty_cycle == 516); // 500 + 16
}
