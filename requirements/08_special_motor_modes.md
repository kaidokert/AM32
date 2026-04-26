# 08 Specialized Motor Modes

This document covers alternative motor control modes that deviate from the standard brushless operation.

## 8.1 Brushed Motor Mode

- **Requirement 8.1.1: Brushed Motor Support**
  - The firmware MUST support driving brushed DC motors by overriding standard brushless commutation and using two-channel PWM control.
  - **C Reference:** `Src/main.c:1615-1645` (`runBrushedLoop`).
  - **Rust Reference:** `downloads/rm32/src/brushed.rs` (`brushed_tick` logic).

- **Requirement 8.1.2: Bidirectional Brushed Control**
  - For brushed motors, the firmware MUST support bidirectional control (forward/reverse) based on throttle input.
  - **C Reference:** `Src/main.c:1621-1631` (Direction selection).
  - **Rust Reference:** `downloads/rm32/src/brushed.rs:43-57` (Bidirectional mapping).

## 8.2 Gimbal Mode

- **Requirement 8.2.1: Direct Angle Mapping**
  - In Gimbal Mode, the input signal MUST map directly to the sinusoidal angle of the magnetic field, rather than motor speed.
  - **C Reference:** `Src/main.c:269-270` (Mode description), `Src/main.c:2270` (Usage in stepper/sine).
  - **Rust Reference:** `downloads/rm32/src/sine.rs` (Support for gimbal angle mapping).

## 8.3 Bench Testing Modes (Fixed Output)

- **Requirement 8.3.1: Fixed Duty Mode**
  - The firmware MUST support a compile-time option to bypass input signals and run at a constant, hardcoded duty cycle.
  - **C Reference:** `Src/main.c:1115-1116` (`#ifdef FIXED_DUTY_MODE`).
  - **Rust Reference:** `downloads/rm32/src/fixed_mode.rs:8-11` (`fixed_duty_input`).

- **Requirement 8.3.2: Fixed Speed Mode**
  - The firmware MUST support a compile-time option to bypass input signals and maintain a constant motor RPM using the speed control PID loop.
  - **C Reference:** `Src/main.c:1786-1791` (`#ifdef FIXED_SPEED_MODE`).
  - **Rust Reference:** `downloads/rm32/src/fixed_mode.rs:16-19` (`fixed_speed_target`).
