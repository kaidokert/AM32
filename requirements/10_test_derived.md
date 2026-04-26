# 10 Blackbox & Test-Derived Requirements

This document captures requirements identified through analysis of blackbox test vectors (`tests/blackbox/vectors/`) and existing unit tests (`tests/`). These represent edge cases, threshold logic, and precise behaviors required for cross-implementation parity.

## 10.1 Timing & Threshold Nuances

- **Requirement 10.1.1: Signal Disarm Timeouts**
  - When Armed, the ESC MUST disarm if no valid signal is received for 500ms.
  - When Unarmed, the ESC MUST reset its input detection if no valid signal is received for 2000ms.
  - **C Reference:** `Src/main.c:1330` (Armed timeout), `Src/main.c:1500` (Unarmed reset).
  - **Rust Reference:** `downloads/rm32/src/constants.rs:13` (`ARMING_TIMEOUT_TICKS`).

- **Requirement 10.1.2: BEMF Timeout Suppression**
  - BEMF timeout (stuck rotor protection) MUST be suppressed if zero-crosses are high (> 100) and throttle is low (< 200), or if the motor is in crawler mode with throttle < 400.
  - **C Reference:** `Src/main.c:1962-1975`.
  - **Rust Reference:** `downloads/rm32/src/control/tick.rs:135-143`.

- **Requirement 10.1.3: Dynamic BEMF Timeout Thresholds**
  - The BEMF timeout window MUST be lenient (100 ticks) at low throttle (< 150) and strict (10 ticks) at high throttle.
  - **C Reference:** `Src/main.c:1977-1980`.
  - **Rust Reference:** `downloads/rm32/src/constants.rs:32-37`.

## 10.2 PID & Scaling Edge Cases

- **Requirement 10.2.1: Current Limit Anti-Windup**
  - The current limiting PID MUST implement integral anti-windup, saturating the integral term at `integral_limit` (typically 20,000) to prevent overshoot.
  - **C Reference:** `Src/main.c:578-583` (`doPidCalculations`).
  - **Rust Reference:** `downloads/rm32/src/pid.rs:34-40` (`integral` clamping).

- **Requirement 10.2.2: PWM ARR Delay (Variable PWM)**
  - When `variable_pwm` is active, the recalculated `tim1_arr` MUST be applied in the control loop cycle immediately following its calculation to ensure stable transition.
  - **C Reference:** `Src/main.c:1893-1907`.
  - **Rust Reference:** `downloads/rm32/src/control/tick.rs:541-558`.

## 10.3 State Transitions

- **Requirement 10.3.1: Polling to Interrupt Handover**
  - The transition from polling mode (old routine) to interrupt-driven commutation MUST occur when zero-crosses exceed 20 AND commutation interval is below 2000, OR when the interval falls below `polling_mode_changeover`.
  - **C Reference:** `Src/main.c:1601-1611`.
  - **Rust Reference:** `downloads/rm32/src/constants.rs:20-23`.

- **Requirement 10.3.2: RC Car Return-to-Center**
  - In RC Car reverse mode, after a braking event, the throttle MUST return to the neutral dead-band before the opposite direction can be engaged.
  - **C Reference:** `Src/main.c:1010-1015`.
  - **Rust Reference:** `downloads/rm32/src/control/tick.rs:343-348`.

## 10.4 Telemetry Formatting

- **Requirement 10.4.1: Consumed Current Integration**
  - Consumed current (mAh) MUST be integrated from the 10kHz actual current samples and updated at a 1Hz rate in the main loop.
  - **C Reference:** `Src/main.c:1955-1958`.
  - **Rust Reference:** `downloads/rm32_stm32/src/main_loop.rs:125-126`.

- **Requirement 10.4.2: DShot eRPM Shift Amount**
  - DShot eRPM telemetry MUST use a 3-bit exponent and 9-bit mantissa, where the exponent is derived from the highest set bit (bit 9 to 15) of the commutation interval.
  - **C Reference:** `Src/dshot.c:248-257`.
  - **Rust Reference:** `downloads/rm32/src/dshot.rs:181-193` (`erpm_to_12bit`).
