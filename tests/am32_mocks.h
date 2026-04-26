#pragma once

#include <catch2/catch_test_macros.hpp>
#include <catch2/trompeloeil.hpp>
#include <cstdint>
#include "test_reset.h"

// Mock class for hardware abstraction functions called by main.c
class AM32HalMock {
public:
    // comparator
    MAKE_MOCK0(getCompOutputLevel, uint8_t());
    MAKE_MOCK0(changeCompInput, void());
    MAKE_MOCK0(enableCompInterrupts, void());
    MAKE_MOCK0(maskPhaseInterrupts, void());

    // phaseouts
    MAKE_MOCK1(comStep, void(int));

    // irq
    MAKE_MOCK0(__enable_irq, void());
    MAKE_MOCK0(__disable_irq, void());
};

extern AM32HalMock* hal_mock;

// RAII helper: resets all firmware globals and installs mock per test
struct MockFixture {
    AM32HalMock mock;
    MockFixture() {
        resetFirmwareState();
        hal_mock = &mock;
    }
    ~MockFixture() { hal_mock = nullptr; }
};
