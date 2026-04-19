#include <stdint.h>
#include "main.h"
#include "eeprom.h"
#include "sounds.h"

// Peripheral instances
TIM_TypeDef _TIM1_inst, _TIM2_inst, _TIM14_inst, _TIM17_inst;
DBGMCU_TypeDef _DBGMCU_inst;
ADC_TypeDef _ADC1_inst;

// Global variables
EEprom_t eepromBuffer;
uint32_t dma_buffer[64];
char EDT_ARM_ENABLE;
char EDT_ARMED;

// Remaining stubs are in stubs_hal.c (mockable HAL functions)
// and stubs_noop.c (non-mockable stubs)

// eeprom
void read_flash_bin(uint8_t* data, uint32_t add, int len) { (void)data; (void)add; (void)len; }
void save_flash_nolib(uint8_t* data, int length, uint32_t add) { (void)data; (void)length; (void)add; }
