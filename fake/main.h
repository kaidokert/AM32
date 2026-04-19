#pragma once
#include <stdint.h>

// Stub register types
typedef struct {
    volatile uint32_t CNT, ARR, PSC, CCR1, CCR2, CCR3, CCR4;
    volatile uint32_t DIER, SR, CR1, CR2, CCMR1, CCMR2, CCER, BDTR, EGR;
} TIM_TypeDef;

typedef struct {
    volatile uint32_t IDCODE;
} DBGMCU_TypeDef;

typedef struct {
    volatile uint32_t ISR, IER, CR, CFGR1, CFGR2, SMPR, TR, CHSELR, DR;
} ADC_TypeDef;

// Stub peripheral instances
extern TIM_TypeDef _TIM1_inst, _TIM2_inst, _TIM14_inst;
extern DBGMCU_TypeDef _DBGMCU_inst;
extern ADC_TypeDef _ADC1_inst;

#define TIM1   (&_TIM1_inst)
#define TIM2   (&_TIM2_inst)
#define TIM14  (&_TIM14_inst)
#define COM_TIMER TIM14
#define DBGMCU (&_DBGMCU_inst)
#define ADC1   (&_ADC1_inst)

// LL ADC stubs
#define LL_ADC_RESOLUTION_12B 0
static inline void LL_ADC_REG_StartConversion(ADC_TypeDef *adc) { (void)adc; }
static inline int32_t __LL_ADC_CALC_TEMPERATURE(uint32_t v, uint32_t raw, uint32_t res) { (void)v; (void)raw; (void)res; return 25; }
