#include "am32_mocks.h"
#include <cstring>

extern "C" {
#include "am32_test_externs.h"
}

// Helper to set a clean baseline state
static void resetSetInputState() {
    memset(&eepromBuffer, 0, sizeof(eepromBuffer));
    newinput = 0;
    adjusted_input = 0;
    input = 0;
    forward = 1;
    armed = 1;
    running = 0;
    dshot = 0;
    servoPwm = 0;
    duty_cycle = 0;
    duty_cycle_setpoint = 0;
    duty_cycle_maximum = 2000;
    use_current_limit_adjust = 2000;
    stall_protection_adjust = 0;
    stepper_sine = 0;
    input_override = 0;
    use_speed_control_loop = 0;
    brushed_direction_set = 0;
    reversing_dead_band = 1;
    return_to_center = 0;
    bemf_timeout_happened = 0;
    bemf_timeout = 10;
    prop_brake_active = 0;
    prop_brake_duty_cycle = 0;
    commutation_interval = 10000;
    reverse_speed_threshold = 500;
    old_routine = 1;
    zero_crosses = 0;
    play_tone_flag = 0;
    last_duty_cycle = 0;
    minimum_duty_cycle = 10;
    min_startup_duty = 100;
    startup_max_duty_cycle = 400;
    step = 1;
    use_current_limit = 0;
    servo_dead_band = 100;
}

// ============================================================
// Unidirectional mode (bi_direction = 0)
// ============================================================

TEST_CASE("setInput unidirectional passes through newinput", "[setinput]") {
    MockFixture f;
    ALLOW_CALL(f.mock, __disable_irq());
    ALLOW_CALL(f.mock, __enable_irq());
    ALLOW_CALL(f.mock, enableCompInterrupts());
    ALLOW_CALL(f.mock, changeCompInput());
    ALLOW_CALL(f.mock, comStep(trompeloeil::_));

    resetSetInputState();
    eepromBuffer.bi_direction = 0;
    newinput = 1000;

    setInput();

    REQUIRE(adjusted_input == 1000);
    REQUIRE(input == 1000);
}

TEST_CASE("setInput unidirectional zero input", "[setinput]") {
    MockFixture f;
    ALLOW_CALL(f.mock, __disable_irq());
    ALLOW_CALL(f.mock, __enable_irq());
    ALLOW_CALL(f.mock, comStep(trompeloeil::_));
    ALLOW_CALL(f.mock, changeCompInput());

    resetSetInputState();
    eepromBuffer.bi_direction = 0;
    newinput = 0;

    setInput();

    REQUIRE(adjusted_input == 0);
    REQUIRE(input == 0);
}

// ============================================================
// Motor start / stop via input threshold
// ============================================================

TEST_CASE("setInput starts motor when input >= 47 and armed", "[setinput]") {
    MockFixture f;
    ALLOW_CALL(f.mock, __disable_irq());
    ALLOW_CALL(f.mock, __enable_irq());
    ALLOW_CALL(f.mock, enableCompInterrupts());
    ALLOW_CALL(f.mock, changeCompInput());
    ALLOW_CALL(f.mock, comStep(trompeloeil::_));
    ALLOW_CALL(f.mock, maskPhaseInterrupts());

    resetSetInputState();
    eepromBuffer.bi_direction = 0;
    newinput = 500;
    armed = 1;
    running = 0;

    setInput();

    REQUIRE(running == 1);
    REQUIRE(duty_cycle_setpoint >= minimum_duty_cycle);
}

TEST_CASE("setInput does not start motor when not armed", "[setinput]") {
    MockFixture f;
    ALLOW_CALL(f.mock, __disable_irq());
    ALLOW_CALL(f.mock, __enable_irq());

    resetSetInputState();
    eepromBuffer.bi_direction = 0;
    newinput = 500;
    armed = 0;

    setInput();

    REQUIRE(running == 0);
}

TEST_CASE("setInput low input triggers brake on stop", "[setinput]") {
    MockFixture f;
    ALLOW_CALL(f.mock, __disable_irq());
    ALLOW_CALL(f.mock, __enable_irq());
    ALLOW_CALL(f.mock, comStep(trompeloeil::_));
    ALLOW_CALL(f.mock, changeCompInput());

    resetSetInputState();
    eepromBuffer.bi_direction = 0;
    eepromBuffer.brake_on_stop = 1;
    newinput = 0;
    armed = 1;
    running = 0;

    REQUIRE_CALL(f.mock, maskPhaseInterrupts()).TIMES(0, 10);

    setInput();

    REQUIRE(input == 0);
    REQUIRE(duty_cycle_setpoint == 0);
}

// ============================================================
// Bemf timeout protection
// ============================================================

TEST_CASE("setInput bemf timeout triggers allOff", "[setinput]") {
    MockFixture f;
    ALLOW_CALL(f.mock, __disable_irq());
    ALLOW_CALL(f.mock, __enable_irq());
    REQUIRE_CALL(f.mock, maskPhaseInterrupts());

    resetSetInputState();
    eepromBuffer.bi_direction = 0;
    eepromBuffer.stuck_rotor_protection = 1;
    newinput = 500;
    bemf_timeout_happened = 20;
    bemf_timeout = 10;

    setInput();

    REQUIRE(input == 0);
    REQUIRE(bemf_timeout_happened == 102);
}

// ============================================================
// Bidirectional dshot mode
// ============================================================

TEST_CASE("setInput bidirectional dshot forward", "[setinput]") {
    MockFixture f;
    ALLOW_CALL(f.mock, __disable_irq());
    ALLOW_CALL(f.mock, __enable_irq());
    ALLOW_CALL(f.mock, enableCompInterrupts());
    ALLOW_CALL(f.mock, changeCompInput());
    ALLOW_CALL(f.mock, comStep(trompeloeil::_));
    ALLOW_CALL(f.mock, maskPhaseInterrupts());

    resetSetInputState();
    eepromBuffer.bi_direction = 1;
    eepromBuffer.dir_reversed = 0;
    dshot = 1;
    forward = 1;
    newinput = 1200; // > 1047, forward direction

    setInput();

    // adjusted_input = ((1200 - 1048) * 2 + 47) - 1 = 350
    REQUIRE(adjusted_input == 350);
}

TEST_CASE("setInput bidirectional dshot reverse", "[setinput]") {
    MockFixture f;
    ALLOW_CALL(f.mock, __disable_irq());
    ALLOW_CALL(f.mock, __enable_irq());
    ALLOW_CALL(f.mock, enableCompInterrupts());
    ALLOW_CALL(f.mock, changeCompInput());
    ALLOW_CALL(f.mock, comStep(trompeloeil::_));
    ALLOW_CALL(f.mock, maskPhaseInterrupts());

    resetSetInputState();
    eepromBuffer.bi_direction = 1;
    eepromBuffer.dir_reversed = 0;
    dshot = 1;
    forward = 0; // already reversed
    newinput = 500; // <= 1047 && > 47, reverse direction

    setInput();

    // adjusted_input = ((500 - 48) * 2 + 47) - 1 = 950
    REQUIRE(adjusted_input == 950);
}

TEST_CASE("setInput bidirectional dshot zero input", "[setinput]") {
    MockFixture f;
    ALLOW_CALL(f.mock, __disable_irq());
    ALLOW_CALL(f.mock, __enable_irq());
    ALLOW_CALL(f.mock, comStep(trompeloeil::_));
    ALLOW_CALL(f.mock, changeCompInput());

    resetSetInputState();
    eepromBuffer.bi_direction = 1;
    dshot = 1;
    newinput = 10; // < 48

    setInput();

    REQUIRE(adjusted_input == 0);
}

// ============================================================
// Bidirectional dshot direction change
// ============================================================

TEST_CASE("setInput bidirectional reverses when slow enough", "[setinput]") {
    MockFixture f;
    ALLOW_CALL(f.mock, __disable_irq());
    ALLOW_CALL(f.mock, __enable_irq());
    ALLOW_CALL(f.mock, enableCompInterrupts());
    ALLOW_CALL(f.mock, changeCompInput());
    ALLOW_CALL(f.mock, comStep(trompeloeil::_));
    REQUIRE_CALL(f.mock, maskPhaseInterrupts());

    resetSetInputState();
    eepromBuffer.bi_direction = 1;
    eepromBuffer.dir_reversed = 0;
    dshot = 1;
    forward = 0; // currently reverse
    newinput = 1200; // requesting forward (> 1047)
    commutation_interval = 10000; // slow enough (> reverse_speed_threshold)
    duty_cycle = 100; // < 200

    setInput();

    REQUIRE(forward == 1); // direction changed
    REQUIRE(zero_crosses == 0); // reset
}

// ============================================================
// Duty cycle clamping
// ============================================================

TEST_CASE("setInput clamps duty cycle to maximum", "[setinput]") {
    MockFixture f;
    ALLOW_CALL(f.mock, __disable_irq());
    ALLOW_CALL(f.mock, __enable_irq());
    ALLOW_CALL(f.mock, enableCompInterrupts());
    ALLOW_CALL(f.mock, changeCompInput());
    ALLOW_CALL(f.mock, comStep(trompeloeil::_));

    resetSetInputState();
    eepromBuffer.bi_direction = 0;
    newinput = 2047;
    armed = 1;
    duty_cycle_maximum = 500;

    setInput();

    REQUIRE(duty_cycle_setpoint <= 500);
}

TEST_CASE("setInput startup duty cycle floor", "[setinput]") {
    MockFixture f;
    ALLOW_CALL(f.mock, __disable_irq());
    ALLOW_CALL(f.mock, __enable_irq());
    ALLOW_CALL(f.mock, enableCompInterrupts());
    ALLOW_CALL(f.mock, changeCompInput());
    ALLOW_CALL(f.mock, comStep(trompeloeil::_));

    resetSetInputState();
    eepromBuffer.bi_direction = 0;
    newinput = 48; // just above threshold
    armed = 1;
    running = 0;
    zero_crosses = 0;
    min_startup_duty = 200;

    setInput();

    // During startup (zero_crosses < 30), duty_cycle_setpoint is floored to min_startup_duty
    REQUIRE(duty_cycle_setpoint >= min_startup_duty);
}

// ============================================================
// Sine start mode
// ============================================================

TEST_CASE("setInput sine start maps input to sine range", "[setinput]") {
    MockFixture f;
    ALLOW_CALL(f.mock, __disable_irq());
    ALLOW_CALL(f.mock, __enable_irq());
    ALLOW_CALL(f.mock, enableCompInterrupts());
    ALLOW_CALL(f.mock, changeCompInput());
    ALLOW_CALL(f.mock, comStep(trompeloeil::_));

    resetSetInputState();
    eepromBuffer.bi_direction = 0;
    eepromBuffer.use_sine_start = 1;
    eepromBuffer.sine_mode_changeover_thottle_level = 10; // changeover at 200
    newinput = 100; // between 30 and 200
    armed = 1;

    setInput();

    // input = map(100, 30, 200, 47, 160), somewhere in that range
    REQUIRE(input >= 47);
    REQUIRE(input <= 160);
}

TEST_CASE("setInput sine start dead band below 30", "[setinput]") {
    MockFixture f;
    ALLOW_CALL(f.mock, __disable_irq());
    ALLOW_CALL(f.mock, __enable_irq());
    ALLOW_CALL(f.mock, comStep(trompeloeil::_));
    ALLOW_CALL(f.mock, changeCompInput());

    resetSetInputState();
    eepromBuffer.bi_direction = 0;
    eepromBuffer.use_sine_start = 1;
    eepromBuffer.sine_mode_changeover_thottle_level = 10;
    newinput = 20; // < 30 dead band
    armed = 1;

    setInput();

    REQUIRE(input == 0);
}

// ============================================================
// Play tone flag
// ============================================================

TEST_CASE("setInput plays tone when input low and flag set", "[setinput]") {
    MockFixture f;
    ALLOW_CALL(f.mock, __disable_irq());
    ALLOW_CALL(f.mock, __enable_irq());
    ALLOW_CALL(f.mock, comStep(trompeloeil::_));
    ALLOW_CALL(f.mock, changeCompInput());

    resetSetInputState();
    eepromBuffer.bi_direction = 0;
    newinput = 0;
    armed = 1;
    play_tone_flag = 3;

    setInput();

    // Flag should be consumed
    REQUIRE(play_tone_flag == 0);
}

// ============================================================
// RC car reverse (servo, bidirectional)
// ============================================================

TEST_CASE("setInput rc_car_reverse servo forward brakes on wrong direction", "[setinput]") {
    MockFixture f;
    ALLOW_CALL(f.mock, __disable_irq());
    ALLOW_CALL(f.mock, __enable_irq());
    ALLOW_CALL(f.mock, comStep(trompeloeil::_));
    ALLOW_CALL(f.mock, changeCompInput());
    ALLOW_CALL(f.mock, enableCompInterrupts());
    ALLOW_CALL(f.mock, maskPhaseInterrupts());

    resetSetInputState();
    eepromBuffer.bi_direction = 1;
    eepromBuffer.rc_car_reverse = 1;
    dshot = 0;
    forward = 0; // wrong direction for forward input
    eepromBuffer.dir_reversed = 0;
    servo_dead_band = 50;
    newinput = 1200; // > 1000 + (50 << 1) = 1100
    return_to_center = 0;

    setInput();

    REQUIRE(prop_brake_active == 1);
    REQUIRE(adjusted_input == 0);
}

TEST_CASE("setInput rc_car_reverse servo return to center clears brake", "[setinput]") {
    MockFixture f;
    ALLOW_CALL(f.mock, __disable_irq());
    ALLOW_CALL(f.mock, __enable_irq());
    ALLOW_CALL(f.mock, comStep(trompeloeil::_));
    ALLOW_CALL(f.mock, changeCompInput());
    ALLOW_CALL(f.mock, enableCompInterrupts());
    ALLOW_CALL(f.mock, maskPhaseInterrupts());

    resetSetInputState();
    eepromBuffer.bi_direction = 1;
    eepromBuffer.rc_car_reverse = 1;
    dshot = 0;
    servo_dead_band = 50;
    newinput = 1000; // in dead band: 900-1100
    prop_brake_active = 1;

    setInput();

    REQUIRE(prop_brake_active == 0);
    REQUIRE(return_to_center == 1);
    REQUIRE(adjusted_input == 0);
}

TEST_CASE("setInput rc_car_reverse dshot forward", "[setinput]") {
    MockFixture f;
    ALLOW_CALL(f.mock, __disable_irq());
    ALLOW_CALL(f.mock, __enable_irq());
    ALLOW_CALL(f.mock, comStep(trompeloeil::_));
    ALLOW_CALL(f.mock, changeCompInput());
    ALLOW_CALL(f.mock, enableCompInterrupts());
    ALLOW_CALL(f.mock, maskPhaseInterrupts());

    resetSetInputState();
    eepromBuffer.bi_direction = 1;
    eepromBuffer.rc_car_reverse = 1;
    dshot = 1;
    forward = 1;
    eepromBuffer.dir_reversed = 0;
    newinput = 1200; // > 1047
    return_to_center = 0;
    prop_brake_active = 0;
    reversing_dead_band = 1;

    setInput();

    // adjusted_input = ((1200 - 1048) * 2 + 47) - 1 = 350
    REQUIRE(adjusted_input == 350);
}

TEST_CASE("setInput rc_car_reverse dshot zero clears brake", "[setinput]") {
    MockFixture f;
    ALLOW_CALL(f.mock, __disable_irq());
    ALLOW_CALL(f.mock, __enable_irq());
    ALLOW_CALL(f.mock, comStep(trompeloeil::_));
    ALLOW_CALL(f.mock, changeCompInput());
    ALLOW_CALL(f.mock, enableCompInterrupts());
    ALLOW_CALL(f.mock, maskPhaseInterrupts());

    resetSetInputState();
    eepromBuffer.bi_direction = 1;
    eepromBuffer.rc_car_reverse = 1;
    dshot = 1;
    newinput = 10; // < 48
    prop_brake_active = 1;

    setInput();

    REQUIRE(prop_brake_active == 0);
    REQUIRE(return_to_center == 1);
    REQUIRE(adjusted_input == 0);
}

// ============================================================
// Comp PWM brake paths
// ============================================================

TEST_CASE("setInput comp_pwm brake on stop with drag brake", "[setinput]") {
    MockFixture f;
    ALLOW_CALL(f.mock, __disable_irq());
    ALLOW_CALL(f.mock, __enable_irq());
    ALLOW_CALL(f.mock, comStep(trompeloeil::_));
    ALLOW_CALL(f.mock, changeCompInput());
    ALLOW_CALL(f.mock, enableCompInterrupts());
    ALLOW_CALL(f.mock, maskPhaseInterrupts());

    resetSetInputState();
    eepromBuffer.bi_direction = 0;
    eepromBuffer.comp_pwm = 1;
    eepromBuffer.brake_on_stop = 1;
    eepromBuffer.drag_brake_strength = 5;
    newinput = 0;
    armed = 1;
    running = 0;

    setInput();

    REQUIRE(duty_cycle_setpoint == 0);
    REQUIRE(prop_brake_active == 1);
}

TEST_CASE("setInput comp_pwm sine start enters stepper mode", "[setinput]") {
    MockFixture f;
    ALLOW_CALL(f.mock, __disable_irq());
    ALLOW_CALL(f.mock, __enable_irq());
    ALLOW_CALL(f.mock, comStep(trompeloeil::_));
    ALLOW_CALL(f.mock, changeCompInput());
    ALLOW_CALL(f.mock, enableCompInterrupts());
    ALLOW_CALL(f.mock, maskPhaseInterrupts());

    resetSetInputState();
    eepromBuffer.bi_direction = 0;
    eepromBuffer.comp_pwm = 1;
    eepromBuffer.use_sine_start = 1;
    eepromBuffer.sine_mode_changeover_thottle_level = 10;
    newinput = 0; // adjusted_input=0 < 30 -> input=0 -> enters low-input path
    armed = 1;
    running = 0;
    stepper_sine = 0;

    setInput();

    REQUIRE(stepper_sine == 1);
}

// ============================================================
// Bidirectional servo (non-rc_car) direction change blocked at speed
// ============================================================

TEST_CASE("setInput bidirectional servo blocks reverse at high speed", "[setinput]") {
    MockFixture f;
    ALLOW_CALL(f.mock, __disable_irq());
    ALLOW_CALL(f.mock, __enable_irq());
    ALLOW_CALL(f.mock, comStep(trompeloeil::_));
    ALLOW_CALL(f.mock, changeCompInput());
    ALLOW_CALL(f.mock, enableCompInterrupts());
    ALLOW_CALL(f.mock, maskPhaseInterrupts());

    resetSetInputState();
    eepromBuffer.bi_direction = 1;
    eepromBuffer.rc_car_reverse = 0;
    dshot = 0;
    servo_dead_band = 50;
    forward = 0; // currently reverse
    eepromBuffer.dir_reversed = 0;
    newinput = 1200; // > 1100, requesting forward
    commutation_interval = 100; // too fast (< reverse_speed_threshold)
    duty_cycle = 500; // >= 200
    reverse_speed_threshold = 500;

    setInput();

    // Should block the reverse: newinput forced to 1000
    REQUIRE(forward == 0); // direction NOT changed
}

TEST_CASE("setInput bidirectional servo allows reverse when slow", "[setinput]") {
    MockFixture f;
    ALLOW_CALL(f.mock, __disable_irq());
    ALLOW_CALL(f.mock, __enable_irq());
    ALLOW_CALL(f.mock, comStep(trompeloeil::_));
    ALLOW_CALL(f.mock, changeCompInput());
    ALLOW_CALL(f.mock, enableCompInterrupts());
    REQUIRE_CALL(f.mock, maskPhaseInterrupts());

    resetSetInputState();
    eepromBuffer.bi_direction = 1;
    eepromBuffer.rc_car_reverse = 0;
    dshot = 0;
    servo_dead_band = 50;
    forward = 0;
    eepromBuffer.dir_reversed = 0;
    newinput = 1200;
    commutation_interval = 10000; // slow enough
    duty_cycle = 100; // < 200
    reverse_speed_threshold = 500;

    setInput();

    REQUIRE(forward == 1); // direction changed
    REQUIRE(zero_crosses == 0);
}

// ============================================================
// Prop brake duty cycle calculation
// ============================================================

TEST_CASE("setInput rc_car prop brake calculates duty from servo", "[setinput]") {
    MockFixture f;
    ALLOW_CALL(f.mock, __disable_irq());
    ALLOW_CALL(f.mock, __enable_irq());
    ALLOW_CALL(f.mock, comStep(trompeloeil::_));
    ALLOW_CALL(f.mock, changeCompInput());
    ALLOW_CALL(f.mock, enableCompInterrupts());
    ALLOW_CALL(f.mock, maskPhaseInterrupts());

    resetSetInputState();
    eepromBuffer.bi_direction = 1;
    eepromBuffer.rc_car_reverse = 1;
    dshot = 0;
    servo_dead_band = 50;
    forward = 0; // requesting forward but going reverse
    eepromBuffer.dir_reversed = 0;
    newinput = 1200; // forward request
    prop_brake_active = 1;
    return_to_center = 0;
    armed = 1;

    setInput();

    REQUIRE(prop_brake_duty_cycle > 0);
}
