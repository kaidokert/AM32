#include "am32_mocks.h"

AM32HalMock* hal_mock = nullptr;

// C wrapper functions that dispatch to the mock object.
// These override the weak/stub implementations from fake/stubs.c
// because object files linked directly into the executable take
// precedence over static library archive members.

extern "C" {

uint8_t getCompOutputLevel(void) {
    return hal_mock ? hal_mock->getCompOutputLevel() : 0;
}

void changeCompInput(void) {
    if (hal_mock) hal_mock->changeCompInput();
}

void enableCompInterrupts(void) {
    if (hal_mock) hal_mock->enableCompInterrupts();
}

void maskPhaseInterrupts(void) {
    if (hal_mock) hal_mock->maskPhaseInterrupts();
}

void comStep(int s) {
    if (hal_mock) hal_mock->comStep(s);
}

void __enable_irq(void) {
    if (hal_mock) hal_mock->__enable_irq();
}

void __disable_irq(void) {
    if (hal_mock) hal_mock->__disable_irq();
}

// Phaseout stubs not currently mocked but needed by main.c
void allOff(void) {}
void fullBrake(void) {}
void allpwm(void) {}
void proportionalBrake(void) {}

} // extern "C"
