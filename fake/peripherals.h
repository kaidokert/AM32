#pragma once
#include <stdint.h>

#define INTERVAL_TIMER_COUNT ((uint32_t)0)
#define RELOAD_WATCHDOG_COUNTER() ((void)0)
#define DISABLE_COM_TIMER_INT() ((void)0)
#define ENABLE_COM_TIMER_INT() ((void)0)
#define SET_AND_ENABLE_COM_INT(time) ((void)(time))
#define SET_INTERVAL_TIMER_COUNT(intertime) ((void)(intertime))
#define SET_PRESCALER_PWM(presc) ((void)(presc))
#define SET_AUTO_RELOAD_PWM(relval) ((void)(relval))
#define SET_DUTY_CYCLE_ALL(newdc) ((void)(newdc))

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
