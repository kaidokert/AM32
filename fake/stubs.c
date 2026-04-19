#include <stdint.h>
#include "main.h"
#include "sounds.h"

// Peripheral instances
TIM_TypeDef _TIM1_inst, _TIM2_inst, _TIM14_inst;
DBGMCU_TypeDef _DBGMCU_inst;
ADC_TypeDef _ADC1_inst;

// Global variables
uint32_t dma_buffer[64];
char EDT_ARM_ENABLE;
char EDT_ARMED;

// ARM intrinsics
void __enable_irq(void) {}
void __disable_irq(void) {}

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
void setInputPullUp(void) {}

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

// dshot
void computeDshotDMA(void) {}
void make_dshot_package(uint16_t t) { (void)t; }

// functions
uint32_t getAbsDif(int a, int b) { return (uint32_t)(a > b ? a - b : b - a); }
long map(long x, long in_min, long in_max, long out_min, long out_max) {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
void delayMillis(uint32_t ms) { (void)ms; }
void delayMicros(uint32_t us) { (void)us; }

// sounds
void setVolume(uint8_t volume) { (void)volume; }
void playStartupTune(void) {}
void playInputTune(void) {}
void playInputTune2(void) {}
void playBeaconTune3(void) {}
void playDefaultTone(void) {}
void playChangedTone(void) {}

// kiss telemetry
void makeInfoPacket(void) {}

// serial telemetry
void send_telem_DMA(uint8_t bytes) { (void)bytes; }

// eeprom
void read_flash_bin(uint8_t* data, uint32_t add, int len) { (void)data; (void)add; (void)len; }
void save_flash_nolib(uint8_t* data, int length, uint32_t add) { (void)data; (void)length; (void)add; }
