#pragma once
#include <stdint.h>

#include "main.h"
#define INTERVAL_TIMER TIM2
#define INTERVAL_TIMER_COUNT (INTERVAL_TIMER->CNT)
#define RELOAD_WATCHDOG_COUNTER() ((void)0)
#define DISABLE_COM_TIMER_INT() ((void)0)
#define ENABLE_COM_TIMER_INT() ((void)0)
#define SET_AND_ENABLE_COM_INT(time) ((void)(time))
#define SET_INTERVAL_TIMER_COUNT(intertime) do { INTERVAL_TIMER->CNT = (intertime); } while(0)
// Observable PWM output captures
extern uint16_t fake_pwm_duty;       // last value written by SET_DUTY_CYCLE_ALL
extern uint16_t fake_pwm_arr;        // last value written by SET_AUTO_RELOAD_PWM
extern uint16_t fake_pwm_prescaler;  // last value written by SET_PRESCALER_PWM
extern uint32_t fake_pwm_duty_count; // number of SET_DUTY_CYCLE_ALL calls

#define SET_PRESCALER_PWM(presc) do { fake_pwm_prescaler = (presc); } while(0)
#define SET_AUTO_RELOAD_PWM(relval) do { fake_pwm_arr = (relval); } while(0)
#define SET_DUTY_CYCLE_ALL(newdc) do { fake_pwm_duty = (newdc); fake_pwm_duty_count++; } while(0)

void initAfterJump(void);
void initCorePeripherals(void);
void enableCorePeripherals(void);
void setPWMCompare1(uint16_t compareone);
void setPWMCompare2(uint16_t comparetwo);
void setPWMCompare3(uint16_t comparethree);
void generatePwmTimerEvent(void);
void resetInputCaptureTimer(void);
void reloadWatchDogCounter(void);
void MX_COMP1_Init(void);
void MX_IWDG_Init(void);
void LED_GPIO_init(void);
void UN_TIM_Init(void);

void NVIC_SetPriority(int irq, int prio);
void NVIC_EnableIRQ(int irq);
void NVIC_DisableIRQ(int irq);
void NVIC_SystemReset(void);
