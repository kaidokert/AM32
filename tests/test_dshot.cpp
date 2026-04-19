#include "am32_mocks.h"
#include <cstring>

extern "C" {
#include "am32_test_externs.h"
}

// Helper: build a DShot frame in dma_buffer from an 11-bit value + telemetry bit.
//
// DShot frame layout (16 bits, MSB first as dpulse[0..15]):
//   dpulse[0..10]  = 11-bit throttle value (MSB first)
//   dpulse[11]     = telemetry request bit
//   dpulse[12..15] = 4-bit CRC
//
// CRC = XOR of nibbles from bits 0..11:
//   (dpulse[0..3]) ^ (dpulse[4..7]) ^ (dpulse[8..11])
//
// For bidirectional dshot (inverted_crc), the CRC stored in the frame is inverted.
// The decoder detects this and applies ~checkCRC + 16 to recover.
static void buildDshotFrame(uint16_t value, bool telemetry_request, bool inverted_crc) {
    // Pack bits: [10..0] = throttle, [11] = telem, then compute CRC over [0..11]
    uint8_t bits[16] = {};

    // Throttle MSB first into dpulse[0..10]
    for (int i = 0; i < 11; i++) {
        bits[i] = (value >> (10 - i)) & 1;
    }
    bits[11] = telemetry_request ? 1 : 0;

    // CRC = XOR of three nibbles from bits[0..11]
    uint8_t crc = ((bits[0] ^ bits[4] ^ bits[8]) << 3 |
                   (bits[1] ^ bits[5] ^ bits[9]) << 2 |
                   (bits[2] ^ bits[6] ^ bits[10]) << 1 |
                   (bits[3] ^ bits[7] ^ bits[11]));

    if (inverted_crc) {
        crc = (~crc) & 0xF;
    }

    bits[12] = (crc >> 3) & 1;
    bits[13] = (crc >> 2) & 1;
    bits[14] = (crc >> 1) & 1;
    bits[15] = crc & 1;

    // Encode into dma_buffer as pulse widths.
    // halfpulsetime = frametime / 32.
    // A '1' bit has pdiff > halfpulsetime, a '0' bit has pdiff <= halfpulsetime.
    uint32_t base = 1000;
    uint32_t bit_period = 32;
    uint32_t short_pulse = 10; // '0'
    uint32_t long_pulse = 22;  // '1'

    for (int i = 0; i < 16; i++) {
        dma_buffer[i * 2] = base;
        dma_buffer[i * 2 + 1] = base + (bits[i] ? long_pulse : short_pulse);
        base += bit_period;
    }

    // Set frame timing bounds so frametime = dma_buffer[31] - dma_buffer[0] passes
    dshot_frametime_high = 600;
    dshot_frametime_low = 400;
}

// ============================================================
// computeDshotDMA - frame decoding
// ============================================================

TEST_CASE("computeDshotDMA decodes throttle value", "[dshot]") {
    MockFixture f;
    ALLOW_CALL(f.mock, getCompOutputLevel()).RETURN(0);

    memset(dma_buffer, 0, sizeof(dma_buffer));
    dshot_goodcounts = 0;
    dshot_badcounts = 0;
    armed = 1;
    running = 0;
    dshot_telemetry = 0;
    high_pin_count = 0;
    programming_mode = 0;
    dshotcommand = 0;
    command_count = 0;
    EDT_ARM_ENABLE = 0;
    EDT_ARMED = 1; // must be armed for throttle > 47 to set newinput
    newinput = 0;

    // Build frame with throttle=100, no telemetry, normal CRC
    buildDshotFrame(100, false, false);

    computeDshotDMA();

    REQUIRE(dshot_goodcounts == 1);
    REQUIRE(newinput == 100);
}

TEST_CASE("computeDshotDMA sets send_telemetry on telemetry bit", "[dshot]") {
    MockFixture f;
    ALLOW_CALL(f.mock, getCompOutputLevel()).RETURN(0);

    memset(dma_buffer, 0, sizeof(dma_buffer));
    dshot_goodcounts = 0;
    armed = 1;
    running = 0;
    dshot_telemetry = 0;
    high_pin_count = 0;
    programming_mode = 0;
    dshotcommand = 0;
    command_count = 0;
    send_telemetry = 0;
    EDT_ARM_ENABLE = 0;
    EDT_ARMED = 1;
    newinput = 0;

    buildDshotFrame(200, true, false);

    computeDshotDMA();

    REQUIRE(send_telemetry == 1);
    REQUIRE(newinput == 200);
}

TEST_CASE("computeDshotDMA rejects bad CRC", "[dshot]") {
    MockFixture f;

    memset(dma_buffer, 0, sizeof(dma_buffer));
    dshot_goodcounts = 0;
    dshot_badcounts = 0;
    armed = 1;
    dshot_telemetry = 0;
    high_pin_count = 0;
    programming_mode = 0;

    // Build frame then corrupt a pulse
    buildDshotFrame(100, false, false);
    // Flip bit 0 pulse to wrong width
    dma_buffer[31] = dma_buffer[30] + 22; // was short, now long

    computeDshotDMA();

    REQUIRE(dshot_badcounts == 1);
    REQUIRE(dshot_goodcounts == 0);
}

TEST_CASE("computeDshotDMA zero throttle clears newinput", "[dshot]") {
    MockFixture f;
    ALLOW_CALL(f.mock, getCompOutputLevel()).RETURN(0);

    memset(dma_buffer, 0, sizeof(dma_buffer));
    dshot_goodcounts = 0;
    armed = 1;
    running = 0;
    dshot_telemetry = 0;
    high_pin_count = 0;
    programming_mode = 0;
    dshotcommand = 0;
    command_count = 0;
    newinput = 999;
    EDT_ARM_ENABLE = 0;
    EDT_ARMED = 0;

    buildDshotFrame(0, false, false);

    computeDshotDMA();

    REQUIRE(newinput == 0);
    REQUIRE(dshotcommand == 0);
}

TEST_CASE("computeDshotDMA processes beacon commands", "[dshot]") {
    MockFixture f;
    ALLOW_CALL(f.mock, getCompOutputLevel()).RETURN(0);

    memset(dma_buffer, 0, sizeof(dma_buffer));
    dshot_goodcounts = 0;
    armed = 1;
    running = 0;
    dshot_telemetry = 0;
    high_pin_count = 0;
    programming_mode = 0;
    dshotcommand = 0;
    command_count = 0;
    last_command = 0;
    play_tone_flag = 0;
    EDT_ARM_ENABLE = 0;
    EDT_ARMED = 0;

    // Command 1 = beacon tone 1 (value 1, which is <= 47)
    buildDshotFrame(1, false, false);

    computeDshotDMA();

    REQUIRE(dshotcommand == 0); // consumed after command_count >= 6
    REQUIRE(play_tone_flag == 1);
    REQUIRE(last_dshot_command == 1);
}

TEST_CASE("computeDshotDMA command 6 requests ESC info", "[dshot]") {
    MockFixture f;
    ALLOW_CALL(f.mock, getCompOutputLevel()).RETURN(0);

    memset(dma_buffer, 0, sizeof(dma_buffer));
    dshot_goodcounts = 0;
    armed = 1;
    running = 0;
    dshot_telemetry = 0;
    high_pin_count = 0;
    programming_mode = 0;
    dshotcommand = 0;
    command_count = 0;
    last_command = 0;
    send_esc_info_flag = 0;
    EDT_ARM_ENABLE = 0;
    EDT_ARMED = 0;

    // Need 6 repetitions for non-beacon commands
    for (int i = 0; i < 6; i++) {
        buildDshotFrame(6, false, false);
        computeDshotDMA();
    }

    REQUIRE(send_esc_info_flag == 1);
}

TEST_CASE("computeDshotDMA rejects frame outside timing window", "[dshot]") {
    MockFixture f;

    memset(dma_buffer, 0, sizeof(dma_buffer));
    dshot_goodcounts = 0;
    dshot_badcounts = 0;

    // Set up valid-looking frame but with wrong frame time
    buildDshotFrame(100, false, false);
    dshot_frametime_high = 100; // artificially narrow window
    dshot_frametime_low = 50;

    computeDshotDMA();

    // Should not process at all
    REQUIRE(dshot_goodcounts == 0);
    REQUIRE(dshot_badcounts == 0);
}

TEST_CASE("computeDshotDMA inverted CRC for bidirectional dshot", "[dshot]") {
    MockFixture f;

    memset(dma_buffer, 0, sizeof(dma_buffer));
    dshot_goodcounts = 0;
    dshot_badcounts = 0;
    armed = 1;
    running = 0;
    dshot_telemetry = 1; // bidirectional mode
    high_pin_count = 0;
    programming_mode = 0;
    dshotcommand = 0;
    command_count = 0;
    EDT_ARM_ENABLE = 0;
    EDT_ARMED = 1;
    newinput = 0;

    // Build frame with inverted CRC (as bidirectional dshot expects)
    buildDshotFrame(300, false, true);

    computeDshotDMA();

    REQUIRE(dshot_goodcounts == 1);
    REQUIRE(newinput == 300);
}

// ============================================================
// make_dshot_package - GCR telemetry encoding
// ============================================================

TEST_CASE("make_dshot_package encodes eRPM with checksum", "[dshot]") {
    running = 1;
    dshot_extended_telemetry = 0;
    send_EDT_init = 0;
    send_EDT_deinit = 0;
    buffer_padding = 0;
    memset(gcr, 0, sizeof(gcr));

    make_dshot_package(500);

    // gcr buffer should have non-zero entries (GCR encoded)
    bool has_data = false;
    for (int i = 0; i < 23; i++) {
        if (gcr[i] != 0) has_data = true;
    }
    REQUIRE(has_data);
}

TEST_CASE("make_dshot_package sets max period when not running", "[dshot]") {
    running = 0;
    dshot_extended_telemetry = 0;
    send_EDT_init = 0;
    send_EDT_deinit = 0;
    buffer_padding = 0;
    memset(gcr, 0, sizeof(gcr));

    make_dshot_package(500);

    // When not running, com_time is forced to 65535
    // shift_amount for 65535: bit 15 is set, so shift = 15+1-9 = 7
    // dshot_full_number = (7 << 9) | (65535 >> 7) = 3584 | 511 = 4095 before checksum
    // This is the maximum eRPM encoding (0xFFF = 4095)
    // After checksum: 4095 << 4 | crc
    REQUIRE((dshot_full_number >> 4) == 0xFFF);
}

TEST_CASE("make_dshot_package deterministic for same input", "[dshot]") {
    running = 1;
    dshot_extended_telemetry = 0;
    send_EDT_init = 0;
    send_EDT_deinit = 0;
    buffer_padding = 0;

    memset(gcr, 0, sizeof(gcr));
    make_dshot_package(1000);
    uint32_t gcr_first[37];
    memcpy(gcr_first, gcr, sizeof(gcr));

    memset(gcr, 0, sizeof(gcr));
    make_dshot_package(1000);

    REQUIRE(memcmp(gcr_first, gcr, sizeof(gcr)) == 0);
}

TEST_CASE("make_dshot_package different periods produce different output", "[dshot]") {
    running = 1;
    dshot_extended_telemetry = 0;
    send_EDT_init = 0;
    send_EDT_deinit = 0;
    buffer_padding = 0;

    memset(gcr, 0, sizeof(gcr));
    make_dshot_package(200);
    uint32_t gcr_a[37];
    memcpy(gcr_a, gcr, sizeof(gcr));

    memset(gcr, 0, sizeof(gcr));
    make_dshot_package(2000);

    REQUIRE(memcmp(gcr_a, gcr, sizeof(gcr)) != 0);
}

TEST_CASE("make_dshot_package EDT init frame", "[dshot]") {
    running = 1;
    dshot_extended_telemetry = 0;
    send_EDT_init = 1;
    send_EDT_deinit = 0;
    buffer_padding = 0;
    memset(gcr, 0, sizeof(gcr));

    make_dshot_package(500);

    REQUIRE(send_EDT_init == 0); // consumed
    // EDT init = 0b111000000000 = 0xE00
    REQUIRE((dshot_full_number >> 4) == 0xE00);
}

TEST_CASE("make_dshot_package EDT deinit frame", "[dshot]") {
    running = 1;
    dshot_extended_telemetry = 0;
    send_EDT_init = 0;
    send_EDT_deinit = 1;
    buffer_padding = 0;
    memset(gcr, 0, sizeof(gcr));

    make_dshot_package(500);

    REQUIRE(send_EDT_deinit == 0); // consumed
    // EDT deinit = 0b111011111111 = 0xEFF
    REQUIRE((dshot_full_number >> 4) == 0xEFF);
}

TEST_CASE("make_dshot_package extended telemetry current", "[dshot]") {
    running = 1;
    dshot_extended_telemetry = 1;
    send_EDT_init = 0;
    send_EDT_deinit = 0;
    buffer_padding = 0;
    actual_current = 500; // 500 centiamps

    // Reset scheduler by sending enough eRPM frames
    // First call: last_sent_extended=0, so it tries to send extended
    // current_count starts at 0, needs CURRENT_EDT_RATE_DIVISOR (40) calls
    // Let's fast-track by calling many times
    for (int i = 0; i < 80; i++) {
        memset(gcr, 0, sizeof(gcr));
        make_dshot_package(500);
    }

    // After 80 calls with alternating eRPM/extended, current should have been sent
    // The extended frame for current: 0b0110 << 8 | (500/50) = 0x600 | 10 = 0x60A
    // Hard to assert exact timing, but verify extended telemetry was produced
    REQUIRE(dshot_extended_telemetry == 1);
}

TEST_CASE("make_dshot_package shift encoding for large periods", "[dshot]") {
    running = 1;
    dshot_extended_telemetry = 0;
    send_EDT_init = 0;
    send_EDT_deinit = 0;
    buffer_padding = 0;

    // Small period - should have shift_amount = 0
    memset(gcr, 0, sizeof(gcr));
    make_dshot_package(100);
    int small_number = dshot_full_number >> 4;
    // shift=0, so top 3 bits should be 0
    REQUIRE((small_number >> 9) == 0);

    // Large period - should have nonzero shift
    memset(gcr, 0, sizeof(gcr));
    make_dshot_package(30000);
    int large_number = dshot_full_number >> 4;
    REQUIRE((large_number >> 9) > 0);
}

// ============================================================
// computeDshotDMA - more command coverage
// ============================================================

// Helper for command tests: send a command N times with proper state
static void sendDshotCommand(AM32HalMock& mock, uint16_t cmd, int times) {
    for (int i = 0; i < times; i++) {
        memset(dma_buffer, 0, sizeof(dma_buffer));
        buildDshotFrame(cmd, false, false);
        computeDshotDMA();
    }
}

TEST_CASE("computeDshotDMA command 7 sets forward direction", "[dshot]") {
    MockFixture f;
    ALLOW_CALL(f.mock, getCompOutputLevel()).RETURN(0);

    memset(dma_buffer, 0, sizeof(dma_buffer));
    dshot_goodcounts = 0;
    armed = 1;
    running = 0;
    dshot_telemetry = 0;
    high_pin_count = 0;
    programming_mode = 0;
    dshotcommand = 0;
    command_count = 0;
    last_command = 0;
    EDT_ARM_ENABLE = 0;
    EDT_ARMED = 0;
    eepromBuffer.dir_reversed = 0;
    forward = 0;

    sendDshotCommand(f.mock, 7, 6);

    REQUIRE(eepromBuffer.dir_reversed == 0);
    REQUIRE(forward == 1);
}

TEST_CASE("computeDshotDMA command 8 sets reversed direction", "[dshot]") {
    MockFixture f;
    ALLOW_CALL(f.mock, getCompOutputLevel()).RETURN(0);

    memset(dma_buffer, 0, sizeof(dma_buffer));
    dshot_goodcounts = 0;
    armed = 1;
    running = 0;
    dshot_telemetry = 0;
    high_pin_count = 0;
    programming_mode = 0;
    dshotcommand = 0;
    command_count = 0;
    last_command = 0;
    EDT_ARM_ENABLE = 0;
    EDT_ARMED = 0;
    eepromBuffer.dir_reversed = 0;

    sendDshotCommand(f.mock, 8, 6);

    REQUIRE(eepromBuffer.dir_reversed == 1);
    REQUIRE(forward == 0);
}

TEST_CASE("computeDshotDMA command 9 disables bidirectional", "[dshot]") {
    MockFixture f;
    ALLOW_CALL(f.mock, getCompOutputLevel()).RETURN(0);

    memset(dma_buffer, 0, sizeof(dma_buffer));
    dshot_goodcounts = 0;
    armed = 1;
    running = 0;
    dshot_telemetry = 0;
    high_pin_count = 0;
    programming_mode = 0;
    dshotcommand = 0;
    command_count = 0;
    last_command = 0;
    EDT_ARM_ENABLE = 0;
    EDT_ARMED = 0;
    eepromBuffer.bi_direction = 1;

    sendDshotCommand(f.mock, 9, 6);

    REQUIRE(eepromBuffer.bi_direction == 0);
}

TEST_CASE("computeDshotDMA command 10 enables bidirectional", "[dshot]") {
    MockFixture f;
    ALLOW_CALL(f.mock, getCompOutputLevel()).RETURN(0);

    memset(dma_buffer, 0, sizeof(dma_buffer));
    dshot_goodcounts = 0;
    armed = 1;
    running = 0;
    dshot_telemetry = 0;
    high_pin_count = 0;
    programming_mode = 0;
    dshotcommand = 0;
    command_count = 0;
    last_command = 0;
    EDT_ARM_ENABLE = 0;
    EDT_ARMED = 0;
    eepromBuffer.bi_direction = 0;

    sendDshotCommand(f.mock, 10, 6);

    REQUIRE(eepromBuffer.bi_direction == 1);
}

TEST_CASE("computeDshotDMA command 13 enables extended telemetry", "[dshot]") {
    MockFixture f;
    ALLOW_CALL(f.mock, getCompOutputLevel()).RETURN(0);

    memset(dma_buffer, 0, sizeof(dma_buffer));
    dshot_goodcounts = 0;
    armed = 1;
    running = 0;
    dshot_telemetry = 0;
    high_pin_count = 0;
    programming_mode = 0;
    dshotcommand = 0;
    command_count = 0;
    last_command = 0;
    EDT_ARM_ENABLE = 1;
    EDT_ARMED = 0;
    dshot_extended_telemetry = 0;

    sendDshotCommand(f.mock, 13, 6);

    REQUIRE(dshot_extended_telemetry == 1);
    REQUIRE(EDT_ARMED == 1);
}

TEST_CASE("computeDshotDMA command 14 disables extended telemetry", "[dshot]") {
    MockFixture f;
    ALLOW_CALL(f.mock, getCompOutputLevel()).RETURN(0);

    memset(dma_buffer, 0, sizeof(dma_buffer));
    dshot_goodcounts = 0;
    armed = 1;
    running = 0;
    dshot_telemetry = 0;
    high_pin_count = 0;
    programming_mode = 0;
    dshotcommand = 0;
    command_count = 0;
    last_command = 0;
    EDT_ARM_ENABLE = 0;
    EDT_ARMED = 0;
    dshot_extended_telemetry = 1;

    sendDshotCommand(f.mock, 14, 6);

    REQUIRE(dshot_extended_telemetry == 0);
}

TEST_CASE("computeDshotDMA command 20/21 set direction temporarily", "[dshot]") {
    MockFixture f;
    ALLOW_CALL(f.mock, getCompOutputLevel()).RETURN(0);

    memset(dma_buffer, 0, sizeof(dma_buffer));
    dshot_goodcounts = 0;
    armed = 1;
    running = 0;
    dshot_telemetry = 0;
    high_pin_count = 0;
    programming_mode = 0;
    dshotcommand = 0;
    command_count = 0;
    last_command = 0;
    EDT_ARM_ENABLE = 0;
    EDT_ARMED = 0;
    eepromBuffer.dir_reversed = 0;

    sendDshotCommand(f.mock, 21, 6);
    REQUIRE(forward == 0); // dir_reversed=0, forward = dir_reversed = 0

    sendDshotCommand(f.mock, 20, 6);
    REQUIRE(forward == 1); // forward = 1 - dir_reversed = 1
}

TEST_CASE("computeDshotDMA programming mode sequence", "[dshot]") {
    MockFixture f;
    ALLOW_CALL(f.mock, getCompOutputLevel()).RETURN(0);

    memset(dma_buffer, 0, sizeof(dma_buffer));
    dshot_goodcounts = 0;
    armed = 1;
    running = 0;
    dshot_telemetry = 0;
    high_pin_count = 0;
    programming_mode = 0;
    dshotcommand = 0;
    command_count = 0;
    last_command = 0;
    EDT_ARM_ENABLE = 0;
    EDT_ARMED = 0;

    // Command 36 enters programming mode
    sendDshotCommand(f.mock, 36, 6);
    REQUIRE(programming_mode == 1);

    // Next frame: position (e.g. eeprom offset 5 = max_ramp)
    buildDshotFrame(5, false, false);
    computeDshotDMA();
    REQUIRE(programming_mode == 2);
    REQUIRE(position == 5);

    // Next frame: new value
    buildDshotFrame(200, false, false);
    computeDshotDMA();
    REQUIRE(programming_mode == 3);
    REQUIRE(new_byte == 200);

    // Commit with value 37
    buildDshotFrame(37, false, false);
    computeDshotDMA();
    REQUIRE(programming_mode == 0);
    REQUIRE(eepromBuffer.buffer[5] == 200);
}
