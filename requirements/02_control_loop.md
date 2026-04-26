# 02 Control Loop & Protection

This document details the main control loop logic, state transitions, and safety protection mechanisms.

## 2.1 State Machine

- **Requirement 2.1.1: Motor Armed State**
  - The motor MUST only spin when an "Armed" state is active, which is triggered after a valid zero-throttle signal is received for a specified duration.
  - **C Reference:** `Src/main.c:1330` (Armed timeout check).
  - **Rust Reference:** `downloads/rm32/src/control/tick.rs:442-452` (Arming logic).

- **Requirement 2.1.2: Motor Running State**
  - The "Running" state MUST be entered when throttle exceeds the minimum threshold and exited when a stop command or protection event occurs.
  - **C Reference:** `Src/main.c:1474` (`if (armed && running)`).
  - **Rust Reference:** `downloads/rm32/src/control/isr_logic.rs:50` (`shared.set_running(true)`).

## 2.2 Safety Protections

- **Requirement 2.2.1: Low Voltage Cutoff (LVC)**
  - The firmware MUST monitor battery voltage and shut down the motor or reduce power if the voltage falls below the configured per-cell or absolute threshold.
  - **C Reference:** `Src/main.c:2061-2070` (LVC logic).
  - **Rust Reference:** `downloads/rm32/src/control/tick.rs:636-654` (LVC check), `downloads/rm32_stm32/src/main_loop.rs:128-142`.

- **Requirement 2.2.2: Stuck Rotor Protection**
  - The motor MUST shut down if a commutation zero-cross is not detected within a timeout window (BEMF timeout).
  - **C Reference:** `Src/main.c:1106-1114` (Stuck rotor check).
  - **Rust Reference:** `downloads/rm32/src/control/tick.rs:135-143` (BEMF timeout protection).

- **Requirement 2.2.3: Desync Detection & Recovery**
  - Sudden changes in commutation interval exceeding a threshold MUST trigger a desync recovery routine, resetting the state to re-synchronize with the motor.
  - **C Reference:** `Src/main.c:2006-2022` (Desync detection).
  - **Rust Reference:** `downloads/rm32_stm32/src/main_loop.rs:72-92` (Desync detection and recovery logic).

- **Requirement 2.2.4: eRPM Throttle Restriction**
  - The maximum allowed duty cycle MUST be restricted at low eRPM levels to prevent excessive current draw and protect the motor.
  - **C Reference:** `Src/main.c:2116-2117` (Mapping k_erpm to duty ceiling).
  - **Rust Reference:** `downloads/rm32/src/control/tick.rs:658-663` (Implemented as part of eRPM logic).

- **Requirement 2.2.5: Temperature Limiting**
  - Power MUST be linearly reduced if the MCU or NTC temperature sensor exceeds the configured safety threshold.
  - **C Reference:** `Src/main.c:2124-2127` (Temperature-based duty reduction).
  - **Rust Reference:** `downloads/rm32/src/control/tick.rs:657-664` (Temperature limiting logic).

- **Requirement 2.2.6: RC Car Reverse & Proportional Braking**
  - The firmware MUST support specialized bidirectional control for RC cars, including "double-tap" to reverse and proportional braking on the first reverse input.
  - **C Reference:** `Src/main.c:950-1050` (RC Car logic).
  - **Rust Reference:** `downloads/rm32/src/control/tick.rs:337-380` (`set_input_servo_rc_car`).
