#include <catch2/catch_test_macros.hpp>
#include <catch2/trompeloeil.hpp>

extern "C" {
#include "am32_test_externs.h"
}

TEST_CASE("getAbsDif returns absolute difference", "[functions]") {
    REQUIRE(getAbsDif(10, 3) == 7);
    REQUIRE(getAbsDif(3, 10) == 7);
    REQUIRE(getAbsDif(5, 5) == 0);
}

TEST_CASE("map scales linearly", "[functions]") {
    REQUIRE(map(50, 0, 100, 0, 1000) == 500);
    REQUIRE(map(0, 0, 100, 0, 1000) == 0);
    REQUIRE(map(100, 0, 100, 0, 1000) == 1000);
}

TEST_CASE("makeTelemPackage encodes fields correctly", "[kiss_telemetry]") {
    memset(aTxBuffer, 0, sizeof(aTxBuffer));
    makeTelemPackage(25, 1680, 500, 100, 3000);

    kiss_telem_pkt_t* pkt = (kiss_telem_pkt_t*)aTxBuffer;
    REQUIRE(pkt->temperature == 25);
    // voltage 1680 = 0x0690
    REQUIRE(pkt->voltage_h == 0x06);
    REQUIRE(pkt->voltage_l == 0x90);
    // current 500 = 0x01F4
    REQUIRE(pkt->current_h == 0x01);
    REQUIRE(pkt->current_l == 0xF4);
    // consumption 100 = 0x0064
    REQUIRE(pkt->consumption_h == 0x00);
    REQUIRE(pkt->consumption_l == 0x64);
    // eRPM 3000 = 0x0BB8
    REQUIRE(pkt->erpm_h == 0x0B);
    REQUIRE(pkt->erpm_l == 0xB8);
    // CRC should be non-zero for this data
    REQUIRE(pkt->crc != 0);
}

TEST_CASE("makeTelemPackage CRC is consistent", "[kiss_telemetry]") {
    makeTelemPackage(30, 1200, 0, 0, 0);
    uint8_t crc1 = aTxBuffer[9];
    makeTelemPackage(30, 1200, 0, 0, 0);
    uint8_t crc2 = aTxBuffer[9];
    REQUIRE(crc1 == crc2);
}

TEST_CASE("makeInfoPacket copies eeprom and adds CRC", "[kiss_telemetry]") {
    // Set some known eeprom values
    memset(&eepromBuffer, 0xAA, sizeof(eepromBuffer));
    makeInfoPacket();
    // First 48 bytes should match eeprom buffer
    for (int i = 0; i < 48; i++) {
        REQUIRE(aTxBuffer[i] == eepromBuffer.buffer[i]);
    }
    // Byte 48 is CRC
    REQUIRE(aTxBuffer[48] != 0);
}

TEST_CASE("doPidCalculations proportional response", "[pid]") {
    fastPID pid = {};
    pid.Kp = 100;
    pid.Ki = 0;
    pid.Kd = 0;
    pid.integral_limit = 10000;
    pid.output_limit = 50000;

    int32_t out = doPidCalculations(&pid, 500, 400);
    // error = actual - target = 100, output = error * Kp = 10000
    REQUIRE(out == 10000);
    REQUIRE(pid.error == 100);
}

TEST_CASE("doPidCalculations output clamping", "[pid]") {
    fastPID pid = {};
    pid.Kp = 1000;
    pid.Ki = 0;
    pid.Kd = 0;
    pid.integral_limit = 10000;
    pid.output_limit = 5000;

    int32_t out = doPidCalculations(&pid, 500, 400);
    // error * Kp = 100 * 1000 = 100000, clamped to 5000
    REQUIRE(out == 5000);

    out = doPidCalculations(&pid, 400, 500);
    // error = -100, output = -100000, clamped to -5000
    REQUIRE(out == -5000);
}

TEST_CASE("doPidCalculations integral accumulation", "[pid]") {
    fastPID pid = {};
    pid.Kp = 0;
    pid.Ki = 10;
    pid.Kd = 0;
    pid.integral_limit = 10000;
    pid.output_limit = 50000;

    // Call 3 times with constant error of 50
    doPidCalculations(&pid, 150, 100);
    doPidCalculations(&pid, 150, 100);
    int32_t out = doPidCalculations(&pid, 150, 100);
    // integral = 50*10 + 50*10 + 50*10 = 1500
    REQUIRE(pid.integral == 1500);
    REQUIRE(out == 1500);
}

TEST_CASE("doPidCalculations integral clamping", "[pid]") {
    fastPID pid = {};
    pid.Kp = 0;
    pid.Ki = 100;
    pid.Kd = 0;
    pid.integral_limit = 1000;
    pid.output_limit = 50000;

    // Large error should clamp integral
    doPidCalculations(&pid, 200, 100);  // integral = 100*100 = 10000 -> clamped to 1000
    REQUIRE(pid.integral == 1000);

    // Negative direction
    pid = {};
    pid.Ki = 100;
    pid.integral_limit = 1000;
    pid.output_limit = 50000;
    doPidCalculations(&pid, 100, 200);  // integral = -100*100 = -10000 -> clamped to -1000
    REQUIRE(pid.integral == -1000);
}

TEST_CASE("doPidCalculations derivative response", "[pid]") {
    fastPID pid = {};
    pid.Kp = 0;
    pid.Ki = 0;
    pid.Kd = 10;
    pid.integral_limit = 10000;
    pid.output_limit = 50000;

    doPidCalculations(&pid, 150, 100); // error=50, last_error was 0, derivative = 10*(50-0) = 500
    REQUIRE(pid.derivative == 500);

    int32_t out = doPidCalculations(&pid, 170, 100); // error=70, derivative = 10*(70-50) = 200
    REQUIRE(pid.derivative == 200);
    REQUIRE(out == 200);
}

// ============================================================
// delayMicros / delayMillis
// ============================================================

// UTILITY_TIMER->CNT is the hardware register backing get_timer_us16().
// We can manipulate it directly since it's our fake TIM17 instance.
// delayMicros busy-loops: while((uint16_t)(CNT - start) < micros) {}
// So if we advance CNT past the target from a thread, it exits.

#include <thread>
#include <chrono>

// Helper: bump the timer CNT from a background thread after a short sleep
static void advanceTimerAsync(volatile uint32_t* cnt, uint16_t advance_by, int delay_us = 500) {
    std::thread([cnt, advance_by, delay_us]() {
        std::this_thread::sleep_for(std::chrono::microseconds(delay_us));
        *cnt += advance_by;
    }).detach();
}

TEST_CASE("delayMicros exits when timer advances past target", "[delay]") {
    extern TIM_TypeDef _TIM17_inst;
    _TIM17_inst.CNT = 0;

    // Advance CNT by 100 after 500us real time
    advanceTimerAsync(&_TIM17_inst.CNT, 100, 500);

    // This should block until CNT - 0 >= 50, which happens when CNT hits 100
    delayMicros(50);

    REQUIRE((uint16_t)_TIM17_inst.CNT >= 50);
}

TEST_CASE("delayMicros handles 16-bit timer wrap", "[delay]") {
    extern TIM_TypeDef _TIM17_inst;
    // Start near 16-bit overflow
    _TIM17_inst.CNT = 65530;

    // Advance by 20, wrapping: 65530 + 20 = 65550 (wraps in uint16_t to 65550 & 0xFFFF = 65550, no wrap yet)
    // Actually advance by 100 to wrap: 65530+100 = 65630, as uint16_t = 65630-65536 = 94
    // (uint16_t)(94 - 65530) = (uint16_t)(-65436) = 100... that's the unsigned wrap math
    // Actually: (uint16_t)(new - old) handles wrap naturally.
    advanceTimerAsync(&_TIM17_inst.CNT, 100, 500);

    delayMicros(50);

    // Should have exited; CNT advanced by 100, delta = 100 >= 50
    uint16_t delta = (uint16_t)(_TIM17_inst.CNT - 65530);
    REQUIRE(delta >= 50);
}

TEST_CASE("delayMillis calls delayMicros multiple times", "[delay]") {
    extern TIM_TypeDef _TIM17_inst;
    _TIM17_inst.CNT = 0;

    // For 2ms delay: delayMillis calls delayMicros(1000) twice
    // We need CNT to advance past 1000 each time
    // Use a thread that keeps incrementing CNT rapidly
    std::atomic<bool> stop{false};
    std::thread ticker([&stop]() {
        extern TIM_TypeDef _TIM17_inst;
        while (!stop) {
            _TIM17_inst.CNT += 500;
            std::this_thread::sleep_for(std::chrono::microseconds(100));
        }
    });

    delayMillis(2);
    stop = true;
    ticker.join();

    REQUIRE(_TIM17_inst.CNT >= 2000);
}

// Minimal trompeloeil smoke test - mock a simple interface
class IPeripherals {
public:
    virtual ~IPeripherals() = default;
    virtual void setPWM(uint16_t duty) = 0;
};

class MockPeripherals : public IPeripherals {
public:
    MAKE_MOCK1(setPWM, void(uint16_t));
};

TEST_CASE("trompeloeil mock works", "[mock]") {
    MockPeripherals mock;
    REQUIRE_CALL(mock, setPWM(42));
    mock.setPWM(42);
}
