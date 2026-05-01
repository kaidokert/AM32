# 03 Signal Processing & Input

This document covers detection and processing of control signals (DShot, Servo).

## Input Detection

- **REQ-SIG-AUTO_DETECT:** Automatically detect input protocol (DShot, Servo) based on pulse timing.
  - **C Reference:** `Src/signal.c:118-200` (`transfercomplete` dispatcher)
  - **Rust Reference:** `downloads/rm32/src/transfer.rs:84`, `downloads/rm32/src/signal.rs:13`

- **REQ-SIG-INPUT_DETECTION:** Detailed timing analysis logic for DShot/Servo protocol differentiation.
  - **C Reference:** `Src/signal.c:173-237` (`checkDshot`, `checkServo`, `detectInput`)
  - **Rust Reference:** `downloads/rm32/src/signal.rs`

- **REQ-SIG-INPUT_DISPATCH:** Core dispatcher for processing input frames when ready.
  - **C Reference:** `Src/main.c:941-954` (`NEED_INPUT_READY` logic)
  - **Rust Reference:** `downloads/rm32/src/transfer.rs`

- **REQ-SIG-INVERTED_EXTI:** Support for inverted input signal polarity.
  - **C Reference:** `Src/main.c:861` (`#ifdef INVERTED_EXTI`)
  - **Rust Reference:** `downloads/rm32_stm32/src/bin/main.rs:81,89,97` (init logic)

## DShot Protocol

- **REQ-SIG-DSHOT_DECODE:** Decode 16-pulse DMA sequence with 4-bit XOR checksum.
  - **C Reference:** `Src/dshot.c:70-98`
  - **Rust Reference:** `downloads/rm32/src/dshot.rs:48-112`

- **REQ-SIG-DSHOT_PROCESS:** Higher-level processing of decoded DShot frames (commands vs throttle).
  - **C Reference:** `Src/main.c:690-698` (`processDshot`)
  - **Rust Reference:** `downloads/rm32/src/transfer.rs`

- **REQ-SIG-DSHOT_INPUT_PROCESSING:** Extracting commands and throttle values, including DroneCAN override logic.
  - **C Reference:** `Src/dshot.c:123-157`
  - **Rust Reference:** `downloads/rm32/src/dshot_commands.rs`

- **REQ-SIG-DSHOT_BIDIR:** Bidirectional DShot telemetry via GCR encoding and inverted CRC.
  - **C Reference:** `Src/dshot.c:166-167`, `Src/dshot.c:266`
  - **Rust Reference:** `downloads/rm32/src/dshot.rs:114-162`, `downloads/rm32/src/transfer.rs:128`

- **REQ-SIG-DSHOT_CALIBRATION:** Calibrate DShot frame timing window via unarmed averaging.
  - **C Reference:** `Src/signal.c:134-142`
  - **Rust Reference:** `downloads/rm32/src/transfer.rs:181-193`

## Servo PWM

- **REQ-SIG-SERVO_PWM:** Measure Servo PWM pulse width and map to throttle resolution.
  - **C Reference:** `Src/signal.c:88-100`
  - **Rust Reference:** `downloads/rm32/src/servo.rs:57-111`

- **REQ-SIG-SERVO_STICK_CAL:** Stick calibration routine to set PWM thresholds.
  - **C Reference:** `Src/main.c:1322-1360`
  - **Rust Reference:** `downloads/rm32/src/transfer.rs:195-212`, `downloads/rm32/src/servo.rs:125-168`
