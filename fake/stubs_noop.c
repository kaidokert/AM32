// Non-mockable stubs - linked by both fake_firmware and tests.
#include <stdint.h>

// NVIC
void NVIC_SetPriority(int irq, int prio) { (void)irq; (void)prio; }
void NVIC_SystemReset(void) {}

// peripherals
void initAfterJump(void) {}
void initCorePeripherals(void) {}
void enableCorePeripherals(void) {}
void setPWMCompare1(uint16_t v) { (void)v; }
void setPWMCompare2(uint16_t v) { (void)v; }
void setPWMCompare3(uint16_t v) { (void)v; }
void generatePwmTimerEvent(void) {}
void resetInputCaptureTimer(void) {}
void MX_IWDG_Init(void) {}

// ADC
void ADC_DMA_Callback(void) {}

// IO
void receiveDshotDma(void) {}
void sendDshotDma(void) {}
void setInputPullUp(void) {}
uint8_t getInputPinState(void) { return 0; }
volatile char out_put;
char ic_timer_prescaler;
uint8_t buffer_padding;

// dshot - real implementations in Src/dshot.c

// sounds
void setVolume(uint8_t volume) { (void)volume; }
void playStartupTune(void) {}
void playInputTune(void) {}
void playInputTune2(void) {}
void playBeaconTune3(void) {}
void playDefaultTone(void) {}
void playChangedTone(void) {}

// serial telemetry
void send_telem_DMA(uint8_t bytes) { (void)bytes; }
