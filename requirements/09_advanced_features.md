# 09 Advanced Features & Hardware Variations

This document details nuanced features, custom timing, and hardware variations.

## Timing & Ramp Customizations

- **REQ-MOTOR-RAMP_CUSTOM:** Support for customizable acceleration/deceleration ramps.
  - **C Reference:** `Src/main.c:1381`, `Src/main.c:1439`
  - **Rust Reference:** `downloads/rm32/src/control/isr_logic.rs:173-190`

- **REQ-MOTOR-VOLTAGE_RAMP:** Scaling acceleration ramp based on current battery voltage.
  - **C Reference:** `Src/main.c:452` (`#ifdef VOLTAGE_BASED_RAMP`)
  - **Rust Reference:** `downloads/rm32/src/control/isr_logic.rs:173-190` (logic), `downloads/rm32_stm32/src/isr_handlers.rs:48` (wiring)

- **REQ-MOTOR-STARTUP_BOOST:** Optional torque boost during initial motor synchronization.
  - **C Reference:** `Src/main.c:1847` (`#ifdef USE_STARTUP_BOOST`)
  - **Rust Reference:** `downloads/rm32_stm32/src/bin/main.rs:201` (logic gated by `BOARD.startup_boost`)

## Hardware Support Variations

- **REQ-HW-PWM_ENABLE_BRIDGE:** Support for drivers using an "Enable" pin alongside PWM.
  - **C Reference:** `Src/main.c:124` (`#ifndef PWM_ENABLE_BRIDGE`)
  - **Rust Reference:** `downloads/rm32_stm32/src/phase.rs:40` (`PhaseDriver::new_bridge`), `board.rs` (`bridge_enable` flag)

## Protocol & Telemetry Variations

- **REQ-SIG-CRSF_INPUT:** Support for CRSF (Crossfire) serial protocol input.
  - **C Reference:** `Src/main.c:1828` (`#ifdef USE_CRSF_INPUT`)
  - **Rust Reference:** `downloads/rm32/src/crsf.rs`

- **REQ-TELEM-SERIAL_UART:** Standard serial telemetry output (e.g., KISS) over UART.
  - **C Reference:** `Src/main.c:449` (`#ifdef USE_SERIAL_TELEMETRY`)
  - **Rust Reference:** `downloads/rm32/src/telemetry.rs`, `downloads/rm32_stm32/src/telemetry_uart.rs`
