# 10 Blackbox & Test-Derived Requirements

This document captures fine-grained requirements from test vectors and unit tests.

## Timing & Threshold Nuances

- **REQ-TEST-SIGNAL_TIMEOUTS:** Disarm after 500ms (Armed) or reset detection after 2000ms (Unarmed).
  - **C Reference:** `Src/main.c:1330`, `Src/main.c:1500`
  - **Rust Reference:** `downloads/rm32/src/constants.rs:13`

- **REQ-TEST-BEMF_TIMEOUT_SUPPRESSION:** Suppress stuck rotor protection at high zero-crosses and low throttle.
  - **C Reference:** `Src/main.c:1962-1975`
  - **Rust Reference:** `downloads/rm32/src/control/tick.rs:135-143`

- **REQ-TEST-DYNAMIC_BEMF_TIMEOUT:** Use lenient timeout (100) at low throttle and strict (10) at high throttle.
  - **C Reference:** `Src/main.c:1977-1980`
  - **Rust Reference:** `downloads/rm32/src/constants.rs:32-37`

## PID & Scaling Edge Cases

- **REQ-TEST-PID_ANTI_WINDUP:** Current limiting PID MUST clamp integral term at `integral_limit`.
  - **C Reference:** `Src/main.c:578-583`
  - **Rust Reference:** `downloads/rm32/src/pid.rs:34-40`

- **REQ-TEST-PWM_ARR_DELAY:** Variable PWM ARR changes take effect in the following control cycle.
  - **C Reference:** `Src/main.c:1893-1907`
  - **Rust Reference:** `downloads/rm32/src/control/tick.rs:541-558`

## State Transitions & Formatting

- **REQ-TEST-POLLING_HANDOVER:** Handover from polling to interrupts based on ZC count and commutation interval.
  - **C Reference:** `Src/main.c:1601-1611`
  - **Rust Reference:** `downloads/rm32/src/constants.rs:20-23`

- **REQ-TEST-RC_CAR_CENTER:** Require return-to-neutral before direction change in RC Car mode.
  - **C Reference:** `Src/main.c:1010-1015`
  - **Rust Reference:** `downloads/rm32/src/control/tick.rs:343-348`

- **REQ-TEST-TELEM_INTEGRATION:** Consumed current mAh integration rate (10kHz sample, 1Hz update).
  - **C Reference:** `Src/main.c:1955-1958`
  - **Rust Reference:** `downloads/rm32_stm32/src/main_loop.rs:125-126`

- **REQ-TEST-DSHOT_ERPM_SHIFT:** DShot eRPM telemetry 3-bit exponent / 9-bit mantissa encoding.
  - **C Reference:** `Src/dshot.c:248-257`
  - **Rust Reference:** `downloads/rm32/src/dshot.rs:181-193`
