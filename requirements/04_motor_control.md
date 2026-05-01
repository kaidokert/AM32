# 04 Motor Control & PID

This document covers commutation, BEMF sensing, and PID control loops.

## Commutation & BEMF

- **REQ-MOTOR-6STEP:** Standard 6-step trapezoidal commutation for 3-phase BLDC motors.
  - **C Reference:** `Src/functions.c:28-100`
  - **Rust Reference:** `downloads/rm32/src/commutation.rs`, `downloads/rm32_stm32/src/phase.rs`

- **REQ-MOTOR-COMMUTATION_TIMING:** Precise tracking and calculation of commutation intervals and advance.
  - **C Reference:** `Src/main.c:473-487` (`PeriodElapsedCallback`)
  - **Rust Reference:** `downloads/rm32/src/control/tick.rs`

- **REQ-MOTOR-BEMF_ZC:** Motor position sensing via Back-EMF zero-crossing detection.
  - **C Reference:** `Src/main.c:1385-1560` (`interruptRoutine`)
  - **Rust Reference:** `downloads/rm32/src/control/isr_logic.rs:218-278`

- **REQ-MOTOR-BEMF_POLLING:** Reading BEMF state during low-speed polling mode with noise filtering.
  - **C Reference:** `Src/main.c:405-442` (`getBemfState`)
  - **Rust Reference:** `downloads/rm32/src/control/isr_logic.rs:118-171`

- **REQ-MOTOR-POLLING_MODE:** Blocking polling (busy-wait) for zero-cross during startup/low RPM.
  - **C Reference:** `Src/main.c:1569-1600` (`zcfoundroutine`)
  - **Rust Reference:** `downloads/rm32/src/control/isr_logic.rs:118-171`

- **REQ-MOTOR-POLLING_ZC_HANDLER:** Specific logic for handling zero-cross detection in polling mode.
  - **C Reference:** `Src/main.c:732-777` (`zcfoundroutine`)
  - **Rust Reference:** `downloads/rm32/src/control/isr_logic.rs`

- **REQ-MOTOR-ADAPTIVE_FILTER:** Noise filter for zero-cross detection MUST scale with motor speed.
  - **C Reference:** `Src/main.c:2131`
  - **Rust Reference:** `downloads/rm32/src/control/tick.rs:627`

- **REQ-MOTOR-BEMF_FILTER_ADAPT:** Dynamic adjustment of BEMF filter thresholds based on ZC count.
  - **C Reference:** `Src/main.c:955-968`
  - **Rust Reference:** `downloads/rm32/src/control/tick.rs`

- **REQ-MOTOR-ADAPTIVE_ADVANCE:** Dynamic scaling of motor timing advance based on RPM and duty cycle.
  - **C Reference:** `Src/main.c:1178-1188`
  - **Rust Reference:** `downloads/rm32/src/control/tick.rs`

- **REQ-MOTOR-STARTUP_SEQUENCE:** Managed transition sequence from motor stop to running state.
  - **C Reference:** `Src/main.c:503-511` (`startMotor`)
  - **Rust Reference:** `downloads/rm32/src/control/tick.rs`

- **REQ-MOTOR-STOP_DETECTION:** Detecting cessation of zero-crossings to trigger stop/reset.
  - **C Reference:** `Src/main.c:1209-1221`
  - **Rust Reference:** `downloads/rm32/src/control/tick.rs`

## Control Loops (PID)

- **REQ-MOTOR-PID_CURRENT:** PID loop to restrict duty cycle and prevent over-current.
  - **C Reference:** `Src/main.c:1404-1414`
  - **Rust Reference:** `downloads/rm32/src/control/tick.rs:461-469`

- **REQ-MOTOR-PID_STALL:** PID loop to boost throttle at extremely low RPM (crawler mode).
  - **C Reference:** `Src/main.c:1415-1425`
  - **Rust Reference:** `downloads/rm32_stm32/src/main_loop.rs:170-178`

## Advanced Modes

- **REQ-MOTOR-SINE_START:** Sinusoidal drive (stepper sine) for smooth startup.
  - **C Reference:** `Src/main.c:1160-1175`
  - **Rust Reference:** `downloads/rm32/src/sine.rs`, `downloads/rm32_stm32/src/bin/main.rs:300`

- **REQ-MOTOR-SINE_STEPPER_CONTROL:** Complex state machine for sinusoidal driving and closed-loop transition.
  - **C Reference:** `Src/main.c:1222-1320`
  - **Rust Reference:** `downloads/rm32/src/control/tick.rs`

- **REQ-MOTOR-SINE_PHASE_STEP:** Managing angular field position during sinusoidal or gimbal drive.
  - **C Reference:** `Src/main.c:700-730` (`advanceincrement`)
  - **Rust Reference:** `downloads/rm32/src/sine.rs`

- **REQ-MOTOR-VARIABLE_PWM:** Adjustable PWM frequency (Fixed, Manual, or Automatic).
  - **C Reference:** `Src/main.c:1893-1907`
  - **Rust Reference:** `downloads/rm32/src/control/tick.rs:541-558`
