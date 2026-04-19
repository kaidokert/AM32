// Mockable HAL stubs - used by fake_firmware only.
// For unit tests, these are replaced by mock dispatchers in am32_mocks.cpp.
#include <stdint.h>

// ARM intrinsics
void __enable_irq(void) {}
void __disable_irq(void) {}

// comparator
void maskPhaseInterrupts(void) {}
void changeCompInput(void) {}
void enableCompInterrupts(void) {}
uint8_t getCompOutputLevel(void) { return 0; }

// phaseouts
void allOff(void) {}
void comStep(int s) { (void)s; }
void fullBrake(void) {}
void allpwm(void) {}
void proportionalBrake(void) {}
