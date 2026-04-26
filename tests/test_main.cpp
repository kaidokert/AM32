#include "am32_mocks.h"
#include <cstring>
#include <thread>
#include <chrono>

extern "C" {
#include "am32_test_externs.h"
}

// ============================================================
// loadEEpromSettings
// ============================================================

TEST_CASE("loadEEpromSettings applies defaults for old eeprom version", "[eeprom]") {
    MockFixture f;
    ALLOW_CALL(f.mock, __disable_irq());
    ALLOW_CALL(f.mock, __enable_irq());

    memset(&eepromBuffer, 0, sizeof(eepromBuffer));
    eepromBuffer.eeprom_version = 0; // older than EEPROM_VERSION (3)

    loadEEpromSettings();

    REQUIRE(eepromBuffer.max_ramp == 160);
    REQUIRE(eepromBuffer.minimum_duty_cycle == 1);
    REQUIRE(eepromBuffer.current_P == 100);
    REQUIRE(eepromBuffer.current_D == 100);
    REQUIRE(eepromBuffer.absolute_voltage_cutoff == 10);
}

TEST_CASE("loadEEpromSettings skips defaults for current eeprom version", "[eeprom]") {
    MockFixture f;
    ALLOW_CALL(f.mock, __disable_irq());
    ALLOW_CALL(f.mock, __enable_irq());

    memset(&eepromBuffer, 0, sizeof(eepromBuffer));
    eepromBuffer.eeprom_version = EEPROM_VERSION;
    eepromBuffer.max_ramp = 50;
    eepromBuffer.motor_kv = 50;
    eepromBuffer.motor_poles = 14;
    eepromBuffer.advance_level = 16;
    eepromBuffer.beep_volume = 5;
    eepromBuffer.limits.temperature = 80;
    eepromBuffer.drag_brake_strength = 5;
    eepromBuffer.driving_brake_strength = 10;
    eepromBuffer.sine_mode_power = 5;
    eepromBuffer.sine_mode_changeover_thottle_level = 10;
    eepromBuffer.input_type = AUTO_IN;

    loadEEpromSettings();

    // max_ramp should NOT be overwritten to 160
    REQUIRE(eepromBuffer.max_ramp == 50);
}

TEST_CASE("loadEEpromSettings advance level conversion", "[eeprom]") {
    MockFixture f;
    ALLOW_CALL(f.mock, __disable_irq());
    ALLOW_CALL(f.mock, __enable_irq());

    memset(&eepromBuffer, 0, sizeof(eepromBuffer));
    eepromBuffer.eeprom_version = EEPROM_VERSION;
    eepromBuffer.motor_kv = 50;
    eepromBuffer.motor_poles = 14;
    eepromBuffer.beep_volume = 5;
    eepromBuffer.limits.temperature = 80;
    eepromBuffer.drag_brake_strength = 5;
    eepromBuffer.driving_brake_strength = 10;
    eepromBuffer.sine_mode_power = 5;
    eepromBuffer.sine_mode_changeover_thottle_level = 10;
    eepromBuffer.input_type = AUTO_IN;
    eepromBuffer.max_ramp = 50;

    // Old format (0-3) should be converted
    eepromBuffer.advance_level = 2;
    loadEEpromSettings();
    REQUIRE(temp_advance == 16); // 2 << 3
    REQUIRE(eepromBuffer.advance_level == 26); // 16 + 10

    // New format (10-42) - subtract 10
    eepromBuffer.advance_level = 20;
    loadEEpromSettings();
    REQUIRE(temp_advance == 10); // 20 - 10

    // Out of range (>42) - default to 16
    eepromBuffer.advance_level = 50;
    loadEEpromSettings();
    REQUIRE(temp_advance == 16);
}

TEST_CASE("loadEEpromSettings input type selection", "[eeprom]") {
    MockFixture f;
    ALLOW_CALL(f.mock, __disable_irq());
    ALLOW_CALL(f.mock, __enable_irq());

    auto setup = [&]() {
        memset(&eepromBuffer, 0, sizeof(eepromBuffer));
        eepromBuffer.eeprom_version = EEPROM_VERSION;
        eepromBuffer.motor_kv = 50;
        eepromBuffer.motor_poles = 14;
        eepromBuffer.advance_level = 16;
        eepromBuffer.beep_volume = 5;
        eepromBuffer.limits.temperature = 80;
        eepromBuffer.drag_brake_strength = 5;
        eepromBuffer.driving_brake_strength = 10;
        eepromBuffer.sine_mode_power = 5;
        eepromBuffer.sine_mode_changeover_thottle_level = 10;
        eepromBuffer.max_ramp = 50;
        dshot = 0;
        servoPwm = 0;
        EDT_ARM_ENABLE = 0;
        EDT_ARMED = 0;
    };

    SECTION("AUTO_IN sets EDT_ARMED") {
        setup();
        eepromBuffer.input_type = AUTO_IN;
        loadEEpromSettings();
        REQUIRE(dshot == 0);
        REQUIRE(servoPwm == 0);
        REQUIRE(EDT_ARMED == 1);
    }

    SECTION("DSHOT_IN sets dshot and EDT_ARMED") {
        setup();
        eepromBuffer.input_type = DSHOT_IN;
        loadEEpromSettings();
        REQUIRE(dshot == 1);
        REQUIRE(EDT_ARMED == 1);
    }

    SECTION("SERVO_IN sets servoPwm") {
        setup();
        eepromBuffer.input_type = SERVO_IN;
        loadEEpromSettings();
        REQUIRE(servoPwm == 1);
    }

    SECTION("EDTARM_IN enables EDT arming") {
        setup();
        eepromBuffer.input_type = EDTARM_IN;
        loadEEpromSettings();
        REQUIRE(EDT_ARM_ENABLE == 1);
        REQUIRE(EDT_ARMED == 0);
        REQUIRE(dshot == 1);
    }
}

TEST_CASE("loadEEpromSettings servo thresholds", "[eeprom]") {
    MockFixture f;
    ALLOW_CALL(f.mock, __disable_irq());
    ALLOW_CALL(f.mock, __enable_irq());

    memset(&eepromBuffer, 0, sizeof(eepromBuffer));
    eepromBuffer.eeprom_version = EEPROM_VERSION;
    eepromBuffer.motor_kv = 50;
    eepromBuffer.motor_poles = 14;
    eepromBuffer.advance_level = 16;
    eepromBuffer.beep_volume = 5;
    eepromBuffer.limits.temperature = 80;
    eepromBuffer.drag_brake_strength = 5;
    eepromBuffer.driving_brake_strength = 10;
    eepromBuffer.sine_mode_power = 5;
    eepromBuffer.sine_mode_changeover_thottle_level = 10;
    eepromBuffer.input_type = AUTO_IN;
    eepromBuffer.max_ramp = 50;

    eepromBuffer.servo.low_threshold = 100;
    eepromBuffer.servo.high_threshold = 50;
    eepromBuffer.servo.neutral = 126;

    loadEEpromSettings();

    REQUIRE(servo_low_threshold == (100 * 2) + 750);   // 950
    REQUIRE(servo_high_threshold == (50 * 2) + 1750);   // 1850
    REQUIRE(servo_neutral == 126 + 1374);                // 1500
}

TEST_CASE("loadEEpromSettings current limit PID", "[eeprom]") {
    MockFixture f;
    ALLOW_CALL(f.mock, __disable_irq());
    ALLOW_CALL(f.mock, __enable_irq());

    memset(&eepromBuffer, 0, sizeof(eepromBuffer));
    eepromBuffer.eeprom_version = EEPROM_VERSION;
    eepromBuffer.motor_kv = 50;
    eepromBuffer.motor_poles = 14;
    eepromBuffer.advance_level = 16;
    eepromBuffer.beep_volume = 5;
    eepromBuffer.limits.temperature = 80;
    eepromBuffer.limits.current = 50;
    eepromBuffer.drag_brake_strength = 5;
    eepromBuffer.driving_brake_strength = 10;
    eepromBuffer.sine_mode_power = 5;
    eepromBuffer.sine_mode_changeover_thottle_level = 10;
    eepromBuffer.input_type = AUTO_IN;
    eepromBuffer.max_ramp = 50;
    eepromBuffer.current_P = 80;
    eepromBuffer.current_I = 10;
    eepromBuffer.current_D = 40;

    loadEEpromSettings();

    REQUIRE(currentPid.Kp == 160);  // 80 * 2
    REQUIRE(currentPid.Ki == 10);
    REQUIRE(currentPid.Kd == 80);   // 40 * 2
    REQUIRE(use_current_limit == 1);
}

// ============================================================
// getSmoothedCurrent
// ============================================================

TEST_CASE("getSmoothedCurrent returns running average", "[current]") {
    memset(readings, 0, sizeof(readings));
    readIndex = 0;
    total = 0;

    // Feed constant value
    ADC_raw_current = 1000;
    for (int i = 0; i < 50; i++) {
        getSmoothedCurrent();
    }
    REQUIRE(getSmoothedCurrent() == 1000);
}

TEST_CASE("getSmoothedCurrent wraps read index", "[current]") {
    memset(readings, 0, sizeof(readings));
    readIndex = 0;
    total = 0;

    ADC_raw_current = 500;
    for (int i = 0; i < 60; i++) {
        getSmoothedCurrent();
    }
    // readIndex should have wrapped
    REQUIRE(readIndex == 10); // 60 % 50
}

// ============================================================
// getBemfState
// ============================================================

TEST_CASE("getBemfState increments bemfcounter on correct polarity", "[bemf]") {
    MockFixture f;

    rising = 1;
    bemfcounter = 0;
    bad_count = 0;
    bad_count_threshold = 10;

    // getCompOutputLevel returns 0 -> !0 = 1 = current_state
    // rising=1 and current_state=1 -> bemfcounter++
    ALLOW_CALL(f.mock, getCompOutputLevel()).RETURN(0);

    getBemfState();
    REQUIRE(bemfcounter == 1);

    getBemfState();
    REQUIRE(bemfcounter == 2);
}

TEST_CASE("getBemfState increments bad_count on wrong polarity", "[bemf]") {
    MockFixture f;

    rising = 1;
    bemfcounter = 5;
    bad_count = 0;
    bad_count_threshold = 10;

    // getCompOutputLevel returns 1 -> !1 = 0 = current_state
    // rising=1 and current_state=0 -> bad_count++
    ALLOW_CALL(f.mock, getCompOutputLevel()).RETURN(1);

    getBemfState();
    REQUIRE(bad_count == 1);
    REQUIRE(bemfcounter == 5); // unchanged
}

TEST_CASE("getBemfState resets bemfcounter when bad_count exceeds threshold", "[bemf]") {
    MockFixture f;

    rising = 1;
    bemfcounter = 10;
    bad_count = 9;
    bad_count_threshold = 10;

    // wrong polarity
    ALLOW_CALL(f.mock, getCompOutputLevel()).RETURN(1);

    getBemfState();
    REQUIRE(bad_count == 10);
    REQUIRE(bemfcounter == 10); // not yet

    getBemfState();
    REQUIRE(bad_count == 11);
    REQUIRE(bemfcounter == 0); // reset!
}

TEST_CASE("getBemfState falling edge detection", "[bemf]") {
    MockFixture f;

    rising = 0;
    bemfcounter = 0;
    bad_count = 0;
    bad_count_threshold = 10;

    // getCompOutputLevel returns 0 -> !0 = 1 = current_state
    // rising=0 and current_state=1 -> bad_count++ (wrong for falling)
    ALLOW_CALL(f.mock, getCompOutputLevel()).RETURN(0);
    getBemfState();
    REQUIRE(bad_count == 1);
    REQUIRE(bemfcounter == 0);

    // getCompOutputLevel returns 1 -> !1 = 0 = current_state
    // rising=0 and current_state=0 -> bemfcounter++ (correct for falling)
    bad_count = 0;
    ALLOW_CALL(f.mock, getCompOutputLevel()).RETURN(1);
    getBemfState();
    REQUIRE(bemfcounter == 1);
}

// ============================================================
// commutate
// ============================================================

TEST_CASE("commutate forward increments step 1-6 and wraps", "[commutate]") {
    MockFixture f;

    forward = 1;
    step = 1;
    prop_brake_active = 0;
    average_interval = 0;
    polling_mode_changeover = 0;

    ALLOW_CALL(f.mock, __disable_irq());
    ALLOW_CALL(f.mock, __enable_irq());
    ALLOW_CALL(f.mock, changeCompInput());
    ALLOW_CALL(f.mock, comStep(trompeloeil::_));

    for (int i = 1; i <= 6; i++) {
        step = i;
        desync_check = 0;
        commutate();
    }
    // After step=6, should wrap to 1
    REQUIRE(step == 1);
    REQUIRE(desync_check == 1);
}

TEST_CASE("commutate reverse decrements step and wraps", "[commutate]") {
    MockFixture f;

    forward = 0;
    step = 1;
    prop_brake_active = 0;
    average_interval = 0;
    polling_mode_changeover = 0;

    ALLOW_CALL(f.mock, __disable_irq());
    ALLOW_CALL(f.mock, __enable_irq());
    ALLOW_CALL(f.mock, changeCompInput());
    ALLOW_CALL(f.mock, comStep(trompeloeil::_));

    desync_check = 0;
    commutate();
    // step was 1, decremented to 0, wraps to 6
    REQUIRE(step == 6);
    REQUIRE(desync_check == 1);
}

TEST_CASE("commutate sets rising based on step parity", "[commutate]") {
    MockFixture f;

    forward = 1;
    prop_brake_active = 0;
    average_interval = 0;
    polling_mode_changeover = 0;

    ALLOW_CALL(f.mock, __disable_irq());
    ALLOW_CALL(f.mock, __enable_irq());
    ALLOW_CALL(f.mock, changeCompInput());
    ALLOW_CALL(f.mock, comStep(trompeloeil::_));

    step = 1; // will become 2
    commutate();
    REQUIRE(rising == 0); // step 2 % 2 == 0

    step = 2; // will become 3
    commutate();
    REQUIRE(rising == 1); // step 3 % 2 == 1
}

TEST_CASE("commutate calls comStep and changeCompInput", "[commutate]") {
    MockFixture f;

    forward = 1;
    step = 3;
    prop_brake_active = 0;
    average_interval = 0;
    polling_mode_changeover = 0;

    ALLOW_CALL(f.mock, __disable_irq());
    ALLOW_CALL(f.mock, __enable_irq());
    REQUIRE_CALL(f.mock, comStep(4)); // step goes 3->4
    REQUIRE_CALL(f.mock, changeCompInput());

    commutate();
}

TEST_CASE("commutate skips comStep when prop_brake_active", "[commutate]") {
    MockFixture f;

    forward = 1;
    step = 3;
    prop_brake_active = 1;
    average_interval = 0;
    polling_mode_changeover = 0;

    ALLOW_CALL(f.mock, __disable_irq());
    ALLOW_CALL(f.mock, __enable_irq());
    REQUIRE_CALL(f.mock, changeCompInput());
    // comStep should NOT be called
    FORBID_CALL(f.mock, comStep(trompeloeil::_));

    commutate();
}

TEST_CASE("commutate clears bemfcounter and zcfound", "[commutate]") {
    MockFixture f;

    forward = 1;
    step = 2;
    prop_brake_active = 0;
    bemfcounter = 99;
    zcfound = 1;
    average_interval = 0;
    polling_mode_changeover = 0;

    ALLOW_CALL(f.mock, __disable_irq());
    ALLOW_CALL(f.mock, __enable_irq());
    ALLOW_CALL(f.mock, changeCompInput());
    ALLOW_CALL(f.mock, comStep(trompeloeil::_));

    commutate();
    REQUIRE(bemfcounter == 0);
    REQUIRE(zcfound == 0);
}

// ============================================================
// PeriodElapsedCallback
// ============================================================

TEST_CASE("PeriodElapsedCallback calls commutate and increments zero_crosses", "[callback]") {
    MockFixture f;

    forward = 1;
    step = 1;
    prop_brake_active = 0;
    old_routine = 0;
    zero_crosses = 0;
    commutation_interval = 2000;
    lastzctime = 100;
    thiszctime = 200;
    temp_advance = 16;
    average_interval = 0;
    polling_mode_changeover = 0;
    memset(&eepromBuffer, 0, sizeof(eepromBuffer));

    ALLOW_CALL(f.mock, __disable_irq());
    ALLOW_CALL(f.mock, __enable_irq());
    ALLOW_CALL(f.mock, changeCompInput());
    ALLOW_CALL(f.mock, comStep(trompeloeil::_));
    REQUIRE_CALL(f.mock, enableCompInterrupts());

    PeriodElapsedCallback();

    REQUIRE(zero_crosses == 1);
}

TEST_CASE("PeriodElapsedCallback caps zero_crosses at 10000", "[callback]") {
    MockFixture f;

    forward = 1;
    step = 1;
    prop_brake_active = 0;
    old_routine = 0;
    zero_crosses = 10000;
    commutation_interval = 2000;
    lastzctime = 100;
    thiszctime = 200;
    temp_advance = 0;
    average_interval = 0;
    polling_mode_changeover = 0;
    memset(&eepromBuffer, 0, sizeof(eepromBuffer));

    ALLOW_CALL(f.mock, __disable_irq());
    ALLOW_CALL(f.mock, __enable_irq());
    ALLOW_CALL(f.mock, changeCompInput());
    ALLOW_CALL(f.mock, comStep(trompeloeil::_));
    ALLOW_CALL(f.mock, enableCompInterrupts());

    PeriodElapsedCallback();

    REQUIRE(zero_crosses == 10000); // did not increment past cap
}

TEST_CASE("PeriodElapsedCallback skips enableCompInterrupts when old_routine", "[callback]") {
    MockFixture f;

    forward = 1;
    step = 1;
    prop_brake_active = 0;
    old_routine = 1; // old routine active
    zero_crosses = 0;
    commutation_interval = 2000;
    lastzctime = 100;
    thiszctime = 200;
    temp_advance = 0;
    average_interval = 0;
    polling_mode_changeover = 0;
    memset(&eepromBuffer, 0, sizeof(eepromBuffer));

    ALLOW_CALL(f.mock, __disable_irq());
    ALLOW_CALL(f.mock, __enable_irq());
    ALLOW_CALL(f.mock, changeCompInput());
    ALLOW_CALL(f.mock, comStep(trompeloeil::_));
    FORBID_CALL(f.mock, enableCompInterrupts());

    PeriodElapsedCallback();
}

// ============================================================
// startMotor
// ============================================================

TEST_CASE("startMotor commutates and sets running when stopped", "[start]") {
    MockFixture f;

    running = 0;
    forward = 1;
    step = 1;
    prop_brake_active = 0;
    average_interval = 0;
    polling_mode_changeover = 0;

    ALLOW_CALL(f.mock, __disable_irq());
    ALLOW_CALL(f.mock, __enable_irq());
    ALLOW_CALL(f.mock, changeCompInput());
    ALLOW_CALL(f.mock, comStep(trompeloeil::_));
    REQUIRE_CALL(f.mock, enableCompInterrupts());

    startMotor();

    REQUIRE(running == 1);
    REQUIRE(commutation_interval == 10000);
}

TEST_CASE("startMotor does not re-commutate when already running", "[start]") {
    MockFixture f;

    running = 1;
    commutation_interval = 500;

    // comStep should not be called since running==1 skips commutate()
    FORBID_CALL(f.mock, comStep(trompeloeil::_));
    REQUIRE_CALL(f.mock, enableCompInterrupts());

    startMotor();

    REQUIRE(running == 1);
    REQUIRE(commutation_interval == 500); // unchanged
}

// ============================================================
// advanceincrement
// ============================================================

TEST_CASE("advanceincrement forward decrements positions", "[advance]") {
    forward = 1;
    phase_A_position = 100;
    phase_B_position = 219;
    phase_C_position = 339;

    advanceincrement();

    REQUIRE(phase_A_position == 99);
    REQUIRE(phase_B_position == 218);
    REQUIRE(phase_C_position == 338);
}

TEST_CASE("advanceincrement reverse increments positions", "[advance]") {
    forward = 0;
    phase_A_position = 100;
    phase_B_position = 219;
    phase_C_position = 339;

    advanceincrement();

    REQUIRE(phase_A_position == 101);
    REQUIRE(phase_B_position == 220);
    REQUIRE(phase_C_position == 340);
}

TEST_CASE("advanceincrement forward wraps 0 to 359", "[advance]") {
    forward = 1;
    phase_A_position = 0;
    phase_B_position = 0;
    phase_C_position = 0;

    advanceincrement();

    REQUIRE(phase_A_position == 359);
    REQUIRE(phase_B_position == 359);
    REQUIRE(phase_C_position == 359);
}

// ============================================================
// zcfoundroutine
// ============================================================

TEST_CASE("zcfoundroutine updates commutation interval and increments zero_crosses", "[zcfound]") {
    MockFixture f;
    ALLOW_CALL(f.mock, __disable_irq());
    ALLOW_CALL(f.mock, __enable_irq());
    ALLOW_CALL(f.mock, changeCompInput());
    ALLOW_CALL(f.mock, comStep(trompeloeil::_));

    forward = 1;
    step = 1;
    prop_brake_active = 0;
    average_interval = 0;
    polling_mode_changeover = 0;
    extern TIM_TypeDef _TIM2_inst;
    _TIM2_inst.CNT = 0;
    commutation_interval = 2000;
    temp_advance = 16;
    zero_crosses = 2; // < 5, so while-loop breaks immediately
    bemfcounter = 10;
    bad_count = 5;
    memset(&eepromBuffer, 0, sizeof(eepromBuffer));

    zcfoundroutine();

    // commutation_interval = (0 + 3*2000) / 4 = 1500 (thiszctime=0 from stub)
    REQUIRE(commutation_interval == 1500);
    REQUIRE(zero_crosses == 3);
    REQUIRE(bemfcounter == 0);
    REQUIRE(bad_count == 0);
}

TEST_CASE("zcfoundroutine calls commutate", "[zcfound]") {
    MockFixture f;
    ALLOW_CALL(f.mock, __disable_irq());
    ALLOW_CALL(f.mock, __enable_irq());
    ALLOW_CALL(f.mock, changeCompInput());
    REQUIRE_CALL(f.mock, comStep(trompeloeil::_)); // commutate calls comStep

    extern TIM_TypeDef _TIM2_inst;
    _TIM2_inst.CNT = 0;
    forward = 1;
    step = 1;
    prop_brake_active = 0;
    average_interval = 0;
    polling_mode_changeover = 0;
    commutation_interval = 2000;
    temp_advance = 0;
    zero_crosses = 0;
    memset(&eepromBuffer, 0, sizeof(eepromBuffer));

    zcfoundroutine();
}

TEST_CASE("zcfoundroutine transitions to interrupt mode with stall protection", "[zcfound]") {
    MockFixture f;
    ALLOW_CALL(f.mock, __disable_irq());
    ALLOW_CALL(f.mock, __enable_irq());
    ALLOW_CALL(f.mock, changeCompInput());
    ALLOW_CALL(f.mock, comStep(trompeloeil::_));
    REQUIRE_CALL(f.mock, enableCompInterrupts());

    extern TIM_TypeDef _TIM14_inst; // COM_TIMER
    forward = 1;
    step = 1;
    prop_brake_active = 0;
    average_interval = 0;
    polling_mode_changeover = 0;
    commutation_interval = 1000; // <= 2000
    temp_advance = 0;
    zero_crosses = 19; // >= 5, so while-loop won't break early
    old_routine = 1;
    memset(&eepromBuffer, 0, sizeof(eepromBuffer));
    eepromBuffer.stall_protection = 1;

    // Bump INTERVAL_TIMER (TIM2) from a thread so the while-loop exits
    extern TIM_TypeDef _TIM2_inst;
    std::thread ticker([&]() {
        for (int i = 0; i < 200; i++) {
            _TIM2_inst.CNT += 100;
            std::this_thread::sleep_for(std::chrono::microseconds(50));
        }
    });

    zcfoundroutine();
    ticker.join();

    REQUIRE(zero_crosses == 20);
    REQUIRE(old_routine == 0);
}

TEST_CASE("zcfoundroutine transitions to interrupt mode by interval", "[zcfound]") {
    MockFixture f;
    ALLOW_CALL(f.mock, __disable_irq());
    ALLOW_CALL(f.mock, __enable_irq());
    ALLOW_CALL(f.mock, changeCompInput());
    ALLOW_CALL(f.mock, comStep(trompeloeil::_));
    REQUIRE_CALL(f.mock, enableCompInterrupts());

    extern TIM_TypeDef _TIM2_inst;
    _TIM2_inst.CNT = 0;
    forward = 1;
    step = 1;
    prop_brake_active = 0;
    average_interval = 0;
    polling_mode_changeover = 5000;
    // commutation_interval will become (0 + 3*1000)/4 = 750, which < 5000
    commutation_interval = 1000;
    temp_advance = 0;
    zero_crosses = 2; // < 5, breaks while loop
    old_routine = 1;
    memset(&eepromBuffer, 0, sizeof(eepromBuffer));
    // stall_protection=0 and rc_car_reverse=0, so interval-based check

    zcfoundroutine();

    REQUIRE(old_routine == 0);
}

TEST_CASE("zcfoundroutine stays in polling mode when interval too large", "[zcfound]") {
    MockFixture f;
    ALLOW_CALL(f.mock, __disable_irq());
    ALLOW_CALL(f.mock, __enable_irq());
    ALLOW_CALL(f.mock, changeCompInput());
    ALLOW_CALL(f.mock, comStep(trompeloeil::_));
    FORBID_CALL(f.mock, enableCompInterrupts());

    extern TIM_TypeDef _TIM2_inst;
    _TIM2_inst.CNT = 0;
    forward = 1;
    step = 1;
    prop_brake_active = 0;
    average_interval = 0;
    polling_mode_changeover = 100;
    // commutation_interval = (0 + 3*10000)/4 = 7500, which >= 100
    commutation_interval = 10000;
    temp_advance = 0;
    zero_crosses = 2; // < 5, breaks while loop
    old_routine = 1;
    memset(&eepromBuffer, 0, sizeof(eepromBuffer));

    zcfoundroutine();

    REQUIRE(old_routine == 1); // stays in polling
}

TEST_CASE("advanceincrement reverse wraps 359 to 0", "[advance]") {
    forward = 0;
    phase_A_position = 359;
    phase_B_position = 359;
    phase_C_position = 359;

    advanceincrement();

    REQUIRE(phase_A_position == 0);
    REQUIRE(phase_B_position == 0);
    REQUIRE(phase_C_position == 0);
}

// ============================================================
// interruptRoutine
// ============================================================

TEST_CASE("interruptRoutine passes when comparator confirms zero cross", "[interrupt]") {
    MockFixture f;
    // getCompOutputLevel() != rising means zero cross confirmed
    // rising=1, comp returns 0 -> 0 != 1, passes filter
    ALLOW_CALL(f.mock, getCompOutputLevel()).RETURN(0);
    REQUIRE_CALL(f.mock, __disable_irq());
    REQUIRE_CALL(f.mock, maskPhaseInterrupts());
    REQUIRE_CALL(f.mock, __enable_irq());

    _TIM2_inst.CNT = 500;
    filter_level = 3;
    rising = 1;
    thiszctime = 100;
    waitTime = 200;

    interruptRoutine();

    REQUIRE(lastzctime == 100); // old thiszctime saved
    REQUIRE(thiszctime == 500); // read from INTERVAL_TIMER_COUNT
}

TEST_CASE("interruptRoutine returns early on false zero cross", "[interrupt]") {
    MockFixture f;
    // getCompOutputLevel() == rising means false alarm -> return early
    ALLOW_CALL(f.mock, getCompOutputLevel()).RETURN(1);
    // These should NOT be called
    FORBID_CALL(f.mock, __disable_irq());
    FORBID_CALL(f.mock, maskPhaseInterrupts());

    filter_level = 1;
    rising = 1; // comp returns 1 == rising -> false alarm

    interruptRoutine();
}

TEST_CASE("interruptRoutine filter_level 0 always passes", "[interrupt]") {
    MockFixture f;
    // With filter_level=0, the for loop doesn't execute, goes straight to masking
    REQUIRE_CALL(f.mock, __disable_irq());
    REQUIRE_CALL(f.mock, maskPhaseInterrupts());
    REQUIRE_CALL(f.mock, __enable_irq());

    _TIM2_inst.CNT = 1000;
    filter_level = 0;
    rising = 1;
    thiszctime = 0;

    interruptRoutine();

    REQUIRE(thiszctime == 1000);
}

// ============================================================
// processDshot
// ============================================================

TEST_CASE("processDshot flag 1 calls computeDshotDMA then setInput", "[processdshot]") {
    MockFixture f;
    ALLOW_CALL(f.mock, __disable_irq());
    ALLOW_CALL(f.mock, __enable_irq());
    ALLOW_CALL(f.mock, comStep(trompeloeil::_));
    ALLOW_CALL(f.mock, changeCompInput());
    ALLOW_CALL(f.mock, enableCompInterrupts());
    ALLOW_CALL(f.mock, maskPhaseInterrupts());
    ALLOW_CALL(f.mock, getCompOutputLevel()).RETURN(0);

    compute_dshot_flag = 1;
    // set up minimal state for setInput
    memset(&eepromBuffer, 0, sizeof(eepromBuffer));
    newinput = 0;
    armed = 0;
    stepper_sine = 0;

    processDshot();

    REQUIRE(compute_dshot_flag == 0);
}

TEST_CASE("processDshot flag 2 calls make_dshot_package and returns", "[processdshot]") {
    MockFixture f;
    // With flag==2, it should NOT call setInput, just make_dshot_package
    ALLOW_CALL(f.mock, __disable_irq());
    ALLOW_CALL(f.mock, __enable_irq());

    compute_dshot_flag = 2;
    running = 0;
    e_com_time = 1000;
    buffer_padding = 0;
    // setInput shouldn't be called; if it were, it would need more setup
    // The return after make_dshot_package prevents setInput

    processDshot();

    REQUIRE(compute_dshot_flag == 0);
}

TEST_CASE("processDshot flag 0 just calls setInput", "[processdshot]") {
    MockFixture f;
    ALLOW_CALL(f.mock, __disable_irq());
    ALLOW_CALL(f.mock, __enable_irq());
    ALLOW_CALL(f.mock, comStep(trompeloeil::_));
    ALLOW_CALL(f.mock, changeCompInput());
    ALLOW_CALL(f.mock, enableCompInterrupts());
    ALLOW_CALL(f.mock, maskPhaseInterrupts());
    ALLOW_CALL(f.mock, getCompOutputLevel()).RETURN(0);

    compute_dshot_flag = 0;
    memset(&eepromBuffer, 0, sizeof(eepromBuffer));
    newinput = 100;
    armed = 1;
    stepper_sine = 0;
    running = 0;

    processDshot();

    // setInput should have run; in unidirectional mode, adjusted_input = newinput
    REQUIRE(adjusted_input == 100);
}
