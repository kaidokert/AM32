#pragma once
#include <stdint.h>

void ADC_DMA_Callback(void);
void enableADC_DMA(void);
void activateADC(void);
void ADC_Init(void);

extern uint16_t ADC_raw_temp;
extern uint16_t ADC_raw_volts;
extern uint16_t ADC_raw_current;
