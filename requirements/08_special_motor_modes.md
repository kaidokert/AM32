# 08 Specialized Motor Modes

This document covers alternative motor control modes.

## Brushed Motor Mode

- **REQ-FEAT-BRUSHED_MODE:** Support for driving brushed DC motors via two-channel PWM.
  - **C Reference:** `Src/main.c:1615-1645` (`runBrushedLoop`)
  - **Rust Reference:** `downloads/rm32/src/brushed.rs`

- **REQ-FEAT-BRUSHED_BIDIR:** Bidirectional forward/reverse control for brushed motors.
  - **C Reference:** `Src/main.c:1621-1631`
  - **Rust Reference:** `downloads/rm32/src/brushed.rs:43-57`

## Gimbal Mode

- **REQ-FEAT-GIMBAL_MODE:** Direct input-to-sinusoidal-angle mapping.
  - **C Reference:** `Src/main.c:269-270`, `Src/main.c:2270`
  - **Rust Reference:** `downloads/rm32/src/sine.rs`

## Bench Testing Modes

- **REQ-FEAT-FIXED_DUTY:** Bypass input signal, run at constant hardcoded duty cycle.
  - **C Reference:** `Src/main.c:1115-1116`
  - **Rust Reference:** `downloads/rm32/src/fixed_mode.rs:8-11`

- **REQ-FEAT-FIXED_SPEED:** Bypass input signal, maintain constant mechanical RPM.
  - **C Reference:** `Src/main.c:1786-1791`
  - **Rust Reference:** `downloads/rm32/src/fixed_mode.rs:16-19`

## Debug & Extra Features

- **REQ-FEAT-PULSE_OUT:** Support for an optional pulse output signal for debugging.
  - **C Reference:** `Src/main.c:456` (`#ifdef USE_PULSE_OUT`)
  - **Rust Reference:** `downloads/rm32_stm32/src/phase.rs:119` (`pulse_toggle`), `downloads/rm32_stm32/src/bin/main.rs:58` (wiring)
