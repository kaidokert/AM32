#include "am32_mocks.h"
#include <cstring>

extern "C" {
#include "am32_test_externs.h"
}

// ============================================================
// checkDshot
// ============================================================

TEST_CASE("checkDshot detects dshot600 signal", "[signal]") {
    dshot = 0;
    servoPwm = 0;
    inputSet = 0;
    smallestnumber = 2; // within 1-4 range for dshot600
    average_signal_pulse = 30; // < 60

    checkDshot();

    REQUIRE(dshot == 1);
    REQUIRE(inputSet == 1);
    REQUIRE(ic_timer_prescaler == 0);
    REQUIRE(buffersize == 32);
}

TEST_CASE("checkDshot detects dshot300 signal", "[signal]") {
    dshot = 0;
    servoPwm = 0;
    inputSet = 0;
    smallestnumber = 5; // within 4-8 range for dshot300
    average_signal_pulse = 60; // < 100

    checkDshot();

    REQUIRE(dshot == 1);
    REQUIRE(inputSet == 1);
    REQUIRE(ic_timer_prescaler == 1);
    REQUIRE(buffersize == 32);
}

TEST_CASE("checkDshot rejects out-of-range signal", "[signal]") {
    dshot = 0;
    servoPwm = 0;
    inputSet = 0;
    smallestnumber = 15; // too large for dshot

    checkDshot();

    REQUIRE(dshot == 0);
    REQUIRE(inputSet == 0);
}

// ============================================================
// checkServo
// ============================================================

TEST_CASE("checkServo detects servo PWM signal", "[signal]") {
    dshot = 0;
    servoPwm = 0;
    inputSet = 0;
    smallestnumber = 1000; // within 200-20000

    checkServo();

    REQUIRE(servoPwm == 1);
    REQUIRE(inputSet == 1);
    REQUIRE(ic_timer_prescaler == CPU_FREQUENCY_MHZ - 1);
    REQUIRE(buffersize == 2);
}

TEST_CASE("checkServo rejects too-small pulse", "[signal]") {
    servoPwm = 0;
    inputSet = 0;
    smallestnumber = 100; // < 200

    checkServo();

    REQUIRE(servoPwm == 0);
    REQUIRE(inputSet == 0);
}

// ============================================================
// detectInput
// ============================================================

TEST_CASE("detectInput finds smallest pulse and calls checkDshot", "[signal]") {
    dshot = 0;
    servoPwm = 0;
    inputSet = 0;
    memset(dma_buffer, 0, sizeof(dma_buffer));

    // Simulate dshot600-like pulse pattern: small increments of ~3
    for (int i = 0; i < 32; i++) {
        dma_buffer[i] = 100 + i * 3;
    }

    detectInput();

    REQUIRE(dshot == 1);
    REQUIRE(inputSet == 1);
}

TEST_CASE("detectInput finds servo signal", "[signal]") {
    dshot = 0;
    servoPwm = 0;
    inputSet = 0;
    memset(dma_buffer, 0, sizeof(dma_buffer));

    // Simulate servo-like pulse pattern: large increments ~1000
    for (int i = 0; i < 32; i++) {
        dma_buffer[i] = 100 + i * 1000;
    }

    detectInput();

    REQUIRE(servoPwm == 1);
    REQUIRE(inputSet == 1);
}

// ============================================================
// computeServoInput
// ============================================================

TEST_CASE("computeServoInput maps mid-range pulse to throttle", "[signal]") {
    memset(&eepromBuffer, 0, sizeof(eepromBuffer));
    eepromBuffer.bi_direction = 0;
    servo_low_threshold = 1100;
    servo_high_threshold = 1900;
    calibration_required = 0;
    newinput = 0;
    max_servo_deviation = 20000; // large so rate limiting doesn't clip
    signaltimeout = 100;

    // Pulse width = dma_buffer[1] - dma_buffer[0] = 1500 (mid-range)
    dma_buffer[0] = 1000;
    dma_buffer[1] = 2500;

    computeServoInput();

    REQUIRE(signaltimeout == 0);
    // map(1500, 1100, 1900, 47, 2047) = 47 + (1500-1100)*(2047-47)/(1900-1100) = 47 + 400*2000/800 = 1047
    REQUIRE(newinput == 1047);
}

TEST_CASE("computeServoInput clamps zero below threshold", "[signal]") {
    memset(&eepromBuffer, 0, sizeof(eepromBuffer));
    eepromBuffer.bi_direction = 0;
    servo_low_threshold = 1100;
    servo_high_threshold = 1900;
    calibration_required = 0;
    newinput = 0;
    max_servo_deviation = 20000;

    // Pulse width = 1050 — below servo_low_threshold
    dma_buffer[0] = 1000;
    dma_buffer[1] = 2050;

    computeServoInput();

    // map(1050, 1100, 1900, 47, 2047) goes negative -> <= 48 -> set to 0
    REQUIRE(newinput == 0);
}

TEST_CASE("computeServoInput rate-limits large jumps", "[signal]") {
    memset(&eepromBuffer, 0, sizeof(eepromBuffer));
    eepromBuffer.bi_direction = 0;
    servo_low_threshold = 1100;
    servo_high_threshold = 1900;
    calibration_required = 0;
    newinput = 500;
    max_servo_deviation = 100;

    // Pulse that maps to ~1047, but newinput=500, deviation=547 > 100
    dma_buffer[0] = 1000;
    dma_buffer[1] = 2500;

    computeServoInput();

    // Should only jump by max_servo_deviation
    REQUIRE(newinput == 600);
}

TEST_CASE("computeServoInput rejects out-of-range pulse", "[signal]") {
    memset(&eepromBuffer, 0, sizeof(eepromBuffer));
    calibration_required = 0;
    servorawinput = 0;
    newinput = 500;
    max_servo_deviation = 100;
    zero_input_count = 10;

    // Pulse width = 500, too short (< 800)
    dma_buffer[0] = 1000;
    dma_buffer[1] = 1500;

    computeServoInput();

    REQUIRE(zero_input_count == 0); // reset
    // servorawinput unchanged (0), rate limiter moves newinput toward 0 by max_servo_deviation
    REQUIRE(newinput == 400);
}

TEST_CASE("computeServoInput bidirectional mode maps below neutral", "[signal]") {
    memset(&eepromBuffer, 0, sizeof(eepromBuffer));
    eepromBuffer.bi_direction = 1;
    servo_low_threshold = 1100;
    servo_high_threshold = 1900;
    servo_neutral = 1500;
    calibration_required = 0;
    newinput = 0;
    max_servo_deviation = 20000;

    // Pulse = 1300, below neutral
    dma_buffer[0] = 1000;
    dma_buffer[1] = 2300;

    computeServoInput();

    // map(1300, 1100, 1500, 0, 1000) = 0 + 200*1000/400 = 500
    REQUIRE(newinput == 500);
}

TEST_CASE("computeServoInput bidirectional mode maps above neutral", "[signal]") {
    memset(&eepromBuffer, 0, sizeof(eepromBuffer));
    eepromBuffer.bi_direction = 1;
    servo_low_threshold = 1100;
    servo_high_threshold = 1900;
    servo_neutral = 1500;
    calibration_required = 0;
    newinput = 0;
    max_servo_deviation = 20000;

    // Pulse = 1700, above neutral
    dma_buffer[0] = 1000;
    dma_buffer[1] = 2700;

    computeServoInput();

    // map(1700, 1501, 1900, 1001, 2000) = 1001 + 199*999/399 ≈ 1499
    REQUIRE(newinput >= 1490);
    REQUIRE(newinput <= 1500);
}
