# 04 Motor Control & PID

This document covers the low-level motor commutation, BEMF sensing, and PID control loops.

## 4.1 Commutation & BEMF

- **Requirement 4.1.1: 6-Step Commutation**
  - The firmware MUST implement standard 6-step trapezoidal commutation to drive 3-phase BLDC motors.
  - **C Reference:** `Src/functions.c:28-100` (`comStep` logic).
  - **Rust Reference:** `downloads/rm32/src/commutation.rs` (`Commutation` state), `downloads/rm32_stm32/src/phase.rs` (HAL driver).

- **Requirement 4.1.2: BEMF Zero-Cross Detection**
  - Motor position MUST be sensed by detecting zero-crossing events of the Back-EMF voltage on the floating phase using internal MCU comparators.
  - **C Reference:** `Src/main.c:1385-1560` (`interruptRoutine`).
  - **Rust Reference:** `downloads/rm32/src/control/isr_logic.rs:218-278` (`bemf_zero_cross` handler).

- **Requirement 4.1.3: Polling Mode (Old Routine)**
  - During startup or low RPM where BEMF signal is weak, the firmware MUST use a blocking polling routine (busy-wait) for zero-cross detection.
  - **C Reference:** `Src/main.c:1569-1600` (`zcfoundroutine`).
  - **Rust Reference:** `downloads/rm32/src/control/isr_logic.rs:118-171` (`bemf_polling`).

- **Requirement 4.1.4: Adaptive BEMF Filter**
  - The noise filter for zero-cross detection MUST be dynamically adjusted based on the current commutation interval (motor speed).
  - **C Reference:** `Src/main.c:2131` (`filter_level = map(...)`).
  - **Rust Reference:** `downloads/rm32/src/control/tick.rs:627` (`map(average_interval, 100, 500, 3, 12)`).

## 4.2 Control Loops (PID)

- **Requirement 4.2.1: Current Limiting PID**
  - A PID controller MUST restrict the duty cycle to prevent exceeding the configured maximum current limit.
  - **C Reference:** `Src/main.c:1404-1414` (Current PID).
  - **Rust Reference:** `downloads/rm32/src/control/tick.rs:461-469` (Current limit PID logic).

- **Requirement 4.2.2: Stall Protection PID**
  - For RC cars and crawlers, a PID loop MUST boost the throttle at extremely low RPM to prevent stalling under load.
  - **C Reference:** `Src/main.c:1415-1425` (Stall protection).
  - **Rust Reference:** `downloads/rm32_stm32/src/main_loop.rs:170-178` (Stall protection PID).

## 4.3 Advanced Modes

- **Requirement 4.3.1: Sine/Gimbal Startup**
  - The firmware MUST support a sinusoidal drive mode (stepper sine) for smooth, high-torque startup.
  - **C Reference:** `Src/main.c:1160-1175` (Sine startup logic), `Src/sine.c`.
  - **Rust Reference:** `downloads/rm32/src/sine.rs` (Sine table and logic), `downloads/rm32_stm32/src/bin/main.rs:300` (Sine step invocation).

- **Requirement 4.3.2: Variable PWM Frequency**
  - The PWM switching frequency MUST be adjustable (Fixed, Manual, or Automatic/Scaled) to optimize for efficiency or noise.
  - **C Reference:** `Src/main.c:1893-1907` (Variable PWM modes).
  - **Rust Reference:** `downloads/rm32/src/control/tick.rs:541-558` (Implementation of Variable PWM modes).
