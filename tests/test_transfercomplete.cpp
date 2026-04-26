#include "am32_mocks.h"
#include <cstring>

extern "C" {
#include "am32_test_externs.h"
}

static void resetTransferState() {
    memset(&eepromBuffer, 0, sizeof(eepromBuffer));
    memset(dma_buffer, 0, sizeof(dma_buffer));
    armed = 0;
    dshot_telemetry = 0;
    out_put = 0;
    inputSet = 0;
    dshot = 0;
    servoPwm = 0;
    compute_dshot_flag = 0;
    adjusted_input = 0;
    newinput = 0;
    zero_input_count = 0;
    average_count = 0;
    average_packet_length = 0;
    enter_calibration_count = 0;
    calibration_required = 0;
    high_calibration_set = 0;
    last_input = 0;
    e_com_time = 1000;
    buffer_padding = 0;
}

// ============================================================
// Armed + dshot_telemetry bidirectional paths
// ============================================================

TEST_CASE("transfercomplete armed+telem out_put=1 sets flag 2", "[transfer]") {
    MockFixture f;

    resetTransferState();
    armed = 1;
    dshot_telemetry = 1;
    out_put = 1;

    transfercomplete();

    REQUIRE(compute_dshot_flag == 2);
}

TEST_CASE("transfercomplete armed+telem out_put=0 sets flag 1", "[transfer]") {
    MockFixture f;

    resetTransferState();
    armed = 1;
    dshot_telemetry = 1;
    out_put = 0;

    transfercomplete();

    REQUIRE(compute_dshot_flag == 1);
}

// ============================================================
// Input detection (inputSet == 0)
// ============================================================

TEST_CASE("transfercomplete inputSet=0 calls detectInput", "[transfer]") {
    MockFixture f;

    resetTransferState();
    inputSet = 0;
    // Set up DMA buffer that looks like dshot
    for (int i = 0; i < 32; i++) dma_buffer[i] = 100 + i * 3;

    transfercomplete();

    // detectInput should have identified dshot and set inputSet=1
    REQUIRE(inputSet == 1);
    REQUIRE(dshot == 1);
}

// ============================================================
// Normal dshot processing (inputSet=1, no telem)
// ============================================================

TEST_CASE("transfercomplete dshot mode processes DMA", "[transfer]") {
    MockFixture f;
    ALLOW_CALL(f.mock, getCompOutputLevel()).RETURN(0);

    resetTransferState();
    inputSet = 1;
    dshot = 1;
    dshot_telemetry = 0;

    transfercomplete();

    // computeDshotDMA was called (may not decode valid frame from zero buffer, that's ok)
}

// ============================================================
// Servo PWM processing
// ============================================================

TEST_CASE("transfercomplete servo mode with pin low computes input", "[transfer]") {
    MockFixture f;
    ALLOW_CALL(f.mock, getCompOutputLevel()).RETURN(0);

    resetTransferState();
    inputSet = 1;
    servoPwm = 1;
    dshot_telemetry = 0;
    // Set up servo-like pulse: 1500us
    dma_buffer[0] = 1000;
    dma_buffer[1] = 2500;
    servo_low_threshold = 1100;
    servo_high_threshold = 1900;
    eepromBuffer.bi_direction = 0;

    transfercomplete();

    REQUIRE(buffersize == 2);
}

// ============================================================
// Unarmed calibration entry
// ============================================================

TEST_CASE("transfercomplete unarmed zero input increments count", "[transfer]") {
    MockFixture f;
    ALLOW_CALL(f.mock, getCompOutputLevel()).RETURN(0);

    resetTransferState();
    inputSet = 1;
    dshot = 1;
    armed = 0;
    adjusted_input = 0;
    calibration_required = 0;
    zero_input_count = 5;

    transfercomplete();

    REQUIRE(zero_input_count == 6);
}

TEST_CASE("transfercomplete unarmed dshot averages packet length", "[transfer]") {
    MockFixture f;
    ALLOW_CALL(f.mock, getCompOutputLevel()).RETURN(0);

    resetTransferState();
    inputSet = 1;
    dshot = 1;
    armed = 0;
    adjusted_input = 0;
    calibration_required = 0;
    zero_input_count = 10; // > 5
    average_count = 0;
    dma_buffer[0] = 100;
    dma_buffer[31] = 600;

    transfercomplete();

    REQUIRE(average_count == 1);
    REQUIRE(average_packet_length == 500);
}

TEST_CASE("transfercomplete unarmed calculates frametime after 8 averages", "[transfer]") {
    MockFixture f;
    ALLOW_CALL(f.mock, getCompOutputLevel()).RETURN(0);

    resetTransferState();
    inputSet = 1;
    dshot = 1;
    armed = 0;
    adjusted_input = 0;
    calibration_required = 0;
    zero_input_count = 10;
    average_count = 7; // will become 8
    average_packet_length = 3500; // 7 * 500
    dma_buffer[0] = 100;
    dma_buffer[31] = 600; // adds 500

    transfercomplete();

    REQUIRE(average_count == 8);
    // average_packet_length = 4000
    // dshot_frametime_high = (4000 >> 3) + (4000 >> 7) = 500 + 31 = 531
    // dshot_frametime_low  = (4000 >> 3) - (4000 >> 7) = 500 - 31 = 469
    REQUIRE(dshot_frametime_high == 531);
    REQUIRE(dshot_frametime_low == 469);
}

TEST_CASE("transfercomplete unarmed high input enters calibration", "[transfer]") {
    MockFixture f;
    ALLOW_CALL(f.mock, getCompOutputLevel()).RETURN(0);

    resetTransferState();
    inputSet = 1;
    dshot = 1;
    armed = 0;
    adjusted_input = 1600; // > 1500
    calibration_required = 0;
    high_calibration_set = 0;
    enter_calibration_count = 50; // at threshold
    last_input = 1600; // same as adjusted_input, diff < 50

    transfercomplete();

    // enter_calibration_count was > 50 and !high_calibration_set
    REQUIRE(calibration_required == 1);
}

// ============================================================
// Telemetry mode (inputSet=1, dshot_telemetry=1, not armed)
// ============================================================

TEST_CASE("transfercomplete telem mode out_put=1 processes dshot", "[transfer]") {
    MockFixture f;
    ALLOW_CALL(f.mock, getCompOutputLevel()).RETURN(0);

    resetTransferState();
    inputSet = 1;
    dshot_telemetry = 1;
    armed = 0;
    out_put = 1;

    transfercomplete();

    // Should call make_dshot_package + computeDshotDMA + receiveDshotDma
    // No crash = pass
}

TEST_CASE("transfercomplete telem mode out_put=0 sends dshot", "[transfer]") {
    MockFixture f;

    resetTransferState();
    inputSet = 1;
    dshot_telemetry = 1;
    armed = 0;
    out_put = 0;

    transfercomplete();

    // Should call sendDshotDma, no crash = pass
}
