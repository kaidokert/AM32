# 09 Advanced Features & Hardware Variations

This document details nuanced features, custom timing logic, and hardware-specific variations.

## 9.1 Timing & Ramp Customizations

- **Requirement 9.1.1: Custom Acceleration Ramps**
  - The firmware MUST support customizable acceleration and deceleration ramps to suit different motor and load characteristics.
  - **C Reference:** `Src/main.c:1381` (`#ifndef CUSTOM_RAMP`), `Src/main.c:1439` (Ramp logic).
  - **Rust Reference:** `downloads/rm32/src/control/isr_logic.rs:173-190` (`ramp_limit` logic).

- **Requirement 9.1.2: Voltage-Based Ramp Scaling**
  - The firmware MUST support scaling the acceleration ramp based on the current battery voltage.
  - **C Reference:** `Src/main.c:452` (`#ifdef VOLTAGE_BASED_RAMP`).
  - **Rust Reference:** Not explicitly implemented as a distinct mode, but ramp logic exists.

- **Requirement 9.1.3: Startup Boost**
  - An optional startup boost feature MUST be available to provide extra torque during the initial motor synchronization.
  - **C Reference:** `Src/main.c:1847` (`#ifdef USE_STARTUP_BOOST`).
  - **Rust Reference:** Not explicitly implemented.

## 9.2 Hardware Sensor & Driver Support

- **Requirement 9.2.1: Hall Sensor Support**
  - The firmware MUST support sensored BLDC operation using internal or external Hall sensors for precise low-speed control.
  - **C Reference:** `Src/main.c:678` (`#ifndef HAS_HALL_SENSORS`).
  - **Rust Reference:** `downloads/rm32/src/config.rs:57` (`use_hall_sensors` field).

- **Requirement 9.2.2: PWM Enable Style Bridges**
  - Support MUST be provided for motor drivers that use an "Enable" pin alongside standard PWM signals.
  - **C Reference:** `Src/main.c:124` (`#ifndef PWM_ENABLE_BRIDGE`).
  - **Rust Reference:** Not explicitly implemented.

- **Requirement 9.2.3: Pulse Output**
  - The firmware MUST support an optional pulse output signal (typically for debugging or external sensor feedback).
  - **C Reference:** `Src/main.c:456` (`#ifdef USE_PULSE_OUT`).
  - **Rust Reference:** Not explicitly implemented.

## 9.3 Signal Variations

- **Requirement 9.3.1: CRSF Input Support**
  - The firmware MUST support the CRSF (Crossfire) serial protocol as an alternative input source.
  - **C Reference:** `Src/main.c:1828` (`#ifdef USE_CRSF_INPUT`).
  - **Rust Reference:** `downloads/rm32/src/crsf.rs` (Complete `CrsfParser` implementation).

- **Requirement 9.3.2: Serial Telemetry**
  - Support MUST be provided for standard serial telemetry output (e.g., KISS telemetry) over a dedicated UART pin.
  - **C Reference:** `Src/main.c:449` (`#ifdef USE_SERIAL_TELEMETRY`).
  - **Rust Reference:** `downloads/rm32/src/telemetry.rs`, `downloads/rm32_stm32/src/telemetry_uart.rs`.
