// Consolidated extern declarations for all firmware globals and functions
// used by unit tests. Include this inside an extern "C" {} block.
//
// Every global from main.c, signal.c, and dshot.c that any test touches
// is declared here exactly once.

#pragma once

#include "main.h"
#include "common.h"
#include "eeprom.h"
#include "signal.h"
#include "targets.h"
#include "version.h"
#include "IO.h"
#include "dshot.h"
#include "kiss_telemetry.h"
#include "functions.h"

// ===================== main.c functions =====================

int32_t doPidCalculations(struct fastPID* pidnow, int actual, int target);
void loadEEpromSettings(void);
void advanceincrement(void);
void zcfoundroutine(void);
void interruptRoutine(void);
void processDshot(void);
void setInput(void);
void startMotor(void);
void commutate(void);
void PeriodElapsedCallback(void);
void getBemfState(void);
uint16_t getSmoothedCurrent(void);
void tenKhzRoutine(void);
void main_loop(void);
void saveEEpromSettings(void);

// ===================== signal.c functions =====================

void detectInput(void);
void checkDshot(void);
void checkServo(void);
void computeServoInput(void);
void computeMSInput(void);
void transfercomplete(void);

// ===================== main.c globals =====================

extern uint8_t drive_by_rpm;
extern uint32_t MAXIMUM_RPM_SPEED_CONTROL;
extern uint32_t MINIMUM_RPM_SPEED_CONTROL;
extern fastPID speedPid, currentPid, stallPid;
extern volatile uint32_t polling_mode_changeover;
extern volatile uint8_t ramp_divider;
extern volatile uint8_t max_ramp_startup;
extern volatile uint8_t max_ramp_low_rpm;
extern volatile uint8_t max_ramp_high_rpm;
extern char send_esc_info_flag;
extern uint32_t eeprom_address;
extern uint16_t prop_brake_duty_cycle;
extern uint16_t ledcounter;
extern uint16_t ramp_count;
extern uint32_t process_time;
extern uint32_t start_process;
extern uint16_t one_khz_loop_counter;
extern volatile uint8_t compute_dshot_flag;
extern uint8_t telemetry_interval_ms;
extern uint8_t temp_advance;
extern uint16_t motor_kv;
extern uint8_t dead_time_override;
extern uint16_t stall_protect_target_interval;
extern uint16_t enter_sine_angle;
extern char do_once_sinemode;
extern uint8_t auto_advance_level;
extern char LOW_VOLTAGE_CUTOFF;
extern uint16_t low_cell_volt_cutoff;
extern char return_to_center;
extern uint16_t target_e_com_time;
extern char use_speed_control_loop;
extern int32_t input_override;
extern int16_t use_current_limit_adjust;
extern char use_current_limit;
extern int32_t stall_protection_adjust;
extern uint32_t MCU_Id;
extern uint32_t REV_Id;
extern uint16_t reverse_speed_threshold;
extern uint32_t desync_happened;
extern char crawler_mode;
extern char low_rpm_throttle_limit;
extern uint16_t low_voltage_count;
extern uint16_t telem_ms_count;
extern uint16_t VOLTAGE_DIVIDER;
extern char cell_count;
extern char brushed_direction_set;
extern uint16_t tenkhzcounter;
extern int32_t consumed_current;
extern int32_t smoothed_raw_current;
extern int16_t actual_current;
extern uint16_t min_startup_duty;
extern char bemf_timeout;
extern char reversing_dead_band;
extern uint8_t max_duty_cycle_change;
extern uint16_t last_duty_cycle;
extern uint16_t duty_cycle_setpoint;
extern char play_tone_flag;
extern uint16_t startup_max_duty_cycle;
extern uint16_t minimum_duty_cycle;
extern char desync_check;
extern uint16_t tim1_arr;
extern uint16_t TIMER1_MAX_ARR;
extern uint16_t duty_cycle_maximum;
extern uint16_t low_rpm_level;
extern uint16_t high_rpm_level;
extern uint16_t throttle_max_at_low_rpm;
extern uint16_t throttle_max_at_high_rpm;
extern uint16_t commutation_intervals[6];
extern volatile uint32_t average_interval;
extern uint32_t last_average_interval;
extern uint16_t ADC_raw_current;
extern uint16_t ADC_raw_volts;
extern uint16_t ADC_raw_temp;
extern int16_t converted_degrees;
extern uint8_t PROCESS_ADC_FLAG;
extern volatile char send_telemetry;
extern char prop_brake_active;
extern volatile char dshot_telemetry;
extern uint8_t last_dshot_command;
extern char old_routine;
extern uint16_t adjusted_input;
extern uint16_t smoothedcurrent;
extern uint8_t readIndex;
extern uint32_t total;
extern uint16_t readings[50];
extern uint8_t bemf_timeout_happened;
extern uint8_t filter_level;
extern uint8_t running;
extern uint16_t advance;
extern volatile char rising;
extern char stepper_sine;
extern char forward;
extern uint8_t stuckcounter;
extern uint8_t bad_count;
extern uint8_t bad_count_threshold;
extern volatile char armed;
extern uint16_t zero_input_count;
extern uint16_t input;
extern volatile uint16_t newinput;
extern volatile char inputSet;
extern char dshot;
extern volatile char servoPwm;
extern volatile uint32_t zero_crosses;
extern volatile uint8_t zcfound;
extern uint8_t min_bemf_counts_up;
extern uint8_t min_bemf_counts_down;
extern volatile uint16_t bemfcounter;
extern volatile uint16_t duty_cycle;
extern char step;
extern volatile uint32_t commutation_interval;
extern volatile uint16_t waitTime;
extern uint16_t signaltimeout;
extern volatile uint16_t lastzctime;
extern volatile uint16_t thiszctime;
extern uint16_t adjusted_duty_cycle;
extern uint16_t k_erpm;
extern uint16_t e_rpm;
extern int e_com_time;
extern uint32_t armed_timeout_count;
extern int16_t phase_A_position;
extern int16_t phase_B_position;
extern int16_t phase_C_position;
extern uint8_t buffer_padding;

// ===================== signal.c globals =====================

extern int max_servo_deviation;
extern int servorawinput;
extern uint16_t smallestnumber;
extern uint8_t enter_calibration_count;
extern uint8_t calibration_required;
extern uint8_t high_calibration_counts;
extern uint8_t high_calibration_set;
extern uint16_t last_input;
extern uint16_t last_high_threshold;
extern uint8_t low_calibration_counts;
extern uint8_t buffersize;
extern uint32_t average_signal_pulse;
extern uint8_t average_count;
extern uint32_t average_packet_length;

// ===================== dshot.c globals =====================

extern int dpulse[16];
extern uint32_t gcr[37];
extern uint32_t gcrnumber;
extern int dshot_full_number;
extern uint16_t dshot_frametime;
extern uint16_t dshot_goodcounts;
extern uint16_t dshot_badcounts;
extern uint8_t dshot_extended_telemetry;
extern uint8_t programming_mode;
extern uint16_t position;
extern uint8_t new_byte;
extern uint8_t command_count;
extern uint8_t last_command;
extern uint8_t high_pin_count;
extern char send_EDT_init;
extern char send_EDT_deinit;

// ===================== IO globals =====================

extern volatile char out_put;

// ===================== fake peripherals =====================

extern TIM_TypeDef _TIM1_inst, _TIM2_inst, _TIM14_inst, _TIM17_inst;
extern DBGMCU_TypeDef _DBGMCU_inst;
extern ADC_TypeDef _ADC1_inst;
extern uint32_t dma_buffer[64];

// PWM output captures (from peripherals.h macros)
extern uint16_t fake_pwm_duty;
extern uint16_t fake_pwm_arr;
extern uint16_t fake_pwm_prescaler;
extern uint32_t fake_pwm_duty_count;
