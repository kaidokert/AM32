#include "main.h"

#ifndef RAMP_SPEED_STARTUP
#define RAMP_SPEED_STARTUP 2 // adjusted 2.14 to match duty cycle change between mcu targets.
#endif

#ifndef RAMP_SPEED_LOW_RPM // below commutation interval of 250us
#define RAMP_SPEED_LOW_RPM 6
#endif

#ifndef RAMP_SPEED_HIGH_RPM
#define RAMP_SPEED_HIGH_RPM 16
#endif


#ifndef EEPROM_START_ADD
#define EEPROM_START_ADD (uint32_t)0x0800F800
#endif

#ifndef DEAD_TIME
#define DEAD_TIME 5 // percent of duty cycle, so 5 means 5 percent
#endif

#ifndef TARGET_STALL_PROTECTION_INTERVAL
#define TARGET_STALL_PROTECTION_INTERVAL 6500
#endif

#define FILE_NAME "fake.bin"
#define FIRMWARE_NAME "FakeStub    "

#define STMICRO
#define MCU_F051
#define USE_SERIAL_TELEMETRY

#define LOOP_FREQUENCY_HZ 20000
#define PID_LOOP_DIVIDER (LOOP_FREQUENCY_HZ / 1000)
#define SINE_DIVIDER 2
#define DRONECAN_SUPPORT 0

#ifndef MILLIVOLT_PER_AMP
#define MILLIVOLT_PER_AMP 20
#endif
#ifndef CURRENT_OFFSET
#define CURRENT_OFFSET 0
#endif
#ifndef TARGET_VOLTAGE_DIVIDER
#define TARGET_VOLTAGE_DIVIDER 110
#endif

#define DSHOT_PRIORITY_THRESHOLD 100
#define IC_DMA_IRQ_NAME 0
#define COM_TIMER_IRQ 0
#define COMPARATOR_IRQ 0

#define CPU_FREQUENCY_MHZ 48


#ifndef NOMINAL_PWM
// use a nominal PWM for commutation via TIM1 of 24kHz
#define NOMINAL_PWM 24000U
#endif

#ifndef TIM1_AUTORELOAD
// calculate commutation timer ARR based on a nominal 24kHz PWM
#define TIM1_AUTORELOAD    ((uint16_t)(CPU_FREQUENCY_MHZ * 1000U * 1000U / NOMINAL_PWM)-1)
#endif


#ifndef TARGET_MIN_BEMF_COUNTS
#define TARGET_MIN_BEMF_COUNTS 2
#endif

#ifndef POLLING_MODE_THRESHOLD
#define POLLING_MODE_THRESHOLD 2000
#endif

void __enable_irq(void);
void __disable_irq(void);

