# Discovery: Uncovered Requirements in `Src/main.c`

This document identifies contiguous blocks of code in `Src/main.c` that lack requirement tracking comments (`/* REQTRK: SLUG */`).

## Uncovered Code Blocks

### 1. EEPROM Write Operation
- **Lines**: 375-388
- **Code**: `saveEEpromSettings` function.
- **Analysis**: Handles the persistence of configuration data to flash memory. This is critical for maintaining user settings across power cycles.
- **Proposed Requirement**: `REQ-CFG-SAVE_TO_FLASH`

### 2. Current Sensing Smoothing
- **Lines**: 390-403
- **Code**: `getSmoothedCurrent` function.
- **Analysis**: Implements a moving average filter for ADC current readings to reduce noise in telemetry and protection loops.
- **Proposed Requirement**: `REQ-TELEM-CURRENT_SMOOTHING`

### 3. BEMF Polling Logic
- **Lines**: 405-442
- **Code**: `getBemfState` function.
- **Analysis**: Logic for reading Back EMF state via GPIO or Comparator when the motor is running at low speeds (polling mode). Includes noise filtering via `bad_count`.
- **Proposed Requirement**: `REQ-MOTOR-BEMF_POLLING`

### 4. Commutation Timing and Advance
- **Lines**: 473-487
- **Code**: `PeriodElapsedCallback` function.
- **Analysis**: Updates commutation intervals and calculates timing for the next commutation step, including phase advance calculations.
- **Proposed Requirement**: `REQ-MOTOR-COMMUTATION_TIMING`

### 5. Motor Startup Sequence
- **Lines**: 503-511
- **Code**: `startMotor` function.
- **Analysis**: High-level sequence to transition the motor from stopped to running state.
- **Proposed Requirement**: `REQ-MOTOR-STARTUP_SEQUENCE`

### 6. DShot Command Dispatch
- **Lines**: 690-698
- **Code**: `processDshot` function.
- **Analysis**: Dispatches DShot DMA processing and handles telemetry package generation flags.
- **Proposed Requirement**: `REQ-SIG-DSHOT_PROCESS`

### 7. Sinusoidal Phase Stepping
- **Lines**: 700-730
- **Code**: `advanceincrement` function.
- **Analysis**: Manages the angular position of the magnetic field during sinusoidal startup or gimbal mode.
- **Proposed Requirement**: `REQ-MOTOR-SINE_PHASE_STEP`

### 8. Polling Mode Zero-Cross Handler
- **Lines**: 732-777
- **Code**: `zcfoundroutine` function.
- **Analysis**: Specific handling for zero-cross detection while in polling mode, including interval estimation and blocking waits.
- **Proposed Requirement**: `REQ-MOTOR-POLLING_ZC_HANDLER`

### 9. Input Signal Dispatcher
- **Lines**: 941-954
- **Code**: `NEED_INPUT_READY` block in `main` loop.
- **Analysis**: Handles input processing when the `input_ready` flag is set by interrupts.
- **Proposed Requirement**: `REQ-SIG-INPUT_DISPATCH`

### 10. Adaptive BEMF Filtering
- **Lines**: 955-968
- **Code**: Adaptive `min_bemf_counts` adjustment in `main` loop.
- **Analysis**: Dynamically adjusts BEMF filter thresholds based on the number of successful zero-crosses to improve startup reliability.
- **Proposed Requirement**: `REQ-MOTOR-BEMF_FILTER_ADAPT`

### 11. Signal Timeout Safety
- **Lines**: 986-1014
- **Code**: Signal timeout logic in `main` loop.
- **Analysis**: Safety feature that resets the ESC if the control signal is lost for more than 0.5s (armed) or 2s (unarmed).
- **Proposed Requirement**: `REQ-SAFE-SIGNAL_TIMEOUT`

### 12. Custom LED Feedback
- **Lines**: 1015-1033
- **Code**: `#ifdef USE_CUSTOM_LED` block.
- **Analysis**: Board-specific GPIO toggling for visual feedback based on throttle position.
- **Proposed Requirement**: `REQ-UI-CUSTOM_LED`

### 13. Telemetry Current Integration
- **Lines**: 1035-1038
- **Code**: `consumed_current` accumulation.
- **Analysis**: Integrates instantaneous current over time to calculate total consumed mAh for telemetry.
- **Proposed Requirement**: `REQ-TELEM-CURRENT_INTEGRATION`

### 14. Adaptive Stall Timeout
- **Lines**: 1040-1061
- **Code**: `bemf_timeout` logic.
- **Analysis**: Adjusts the stall detection timeout based on current throttle and motor state.
- **Proposed Requirement**: `REQ-PROT-STALL_TIMEOUT_ADAPT`

### 15. Desync Detection Logic
- **Lines**: 1063-1080
- **Code**: Desync check in `main` loop.
- **Analysis**: Compares commutation intervals to detect sudden deviations that indicate a motor desync.
- **Proposed Requirement**: `REQ-PROT-DESYNC_DETECTION`

### 16. Dynamic IRQ Priority Management
- **Lines**: 1084-1096
- **Code**: NVIC priority adjustment block.
- **Analysis**: Dynamically shifts interrupt priorities between DShot and Commutation based on RPM to ensure timing accuracy at high speeds.
- **Proposed Requirement**: `REQ-RES-IRQ_PRIORITY_DYNAMIC`

### 17. ADC Signal Processing
- **Lines**: 1110-1153
- **Code**: `PROCESS_ADC_FLAG` block (excluding already tracked LVC/NTC).
- **Analysis**: Core logic for converting raw ADC values into physical units (Voltage, Current, Temperature).
- **Proposed Requirement**: `REQ-HW-ADC_PROCESSING`

### 18. Adaptive Motor Advance
- **Lines**: 1178-1188
- **Code**: Filter level and auto-advance adjustment.
- **Analysis**: Dynamically scales motor timing advance and BEMF filtering based on RPM and duty cycle.
- **Proposed Requirement**: `REQ-MOTOR-ADAPTIVE_ADVANCE`

### 19. Motor Stop Detection
- **Lines**: 1209-1221
- **Code**: Long interval timeout check.
- **Analysis**: Detects motor stoppage when zero-crossings cease, triggering a return to polling/startup mode.
- **Proposed Requirement**: `REQ-MOTOR-STOP_DETECTION`

### 20. Sinusoidal Stepper and Gimbal Control
- **Lines**: 1222-1320
- **Code**: Stepper sine and Gimbal mode logic.
- **Analysis**: Complex state machine for open-loop sinusoidal driving and transition to closed-loop BEMF control.
- **Proposed Requirement**: `REQ-MOTOR-SINE_STEPPER_CONTROL`

## Dead Code Identification

### 1. Crawler Mode Flag
- **Lines**: 296, 1052
- **Code**: `char crawler_mode = 0;` and its usage in stall protection logic.
- **Status**: **DEAD CODE**.
- **Reason**: The variable is initialized to 0 and is not loaded from EEPROM or modified anywhere in the source code or tests. The comment explicitly states `// no longer used //`.
