# 03 Signal Processing & Input

This document covers the detection and processing of external control signals (DShot, Servo PWM).

## 3.1 Input Detection & Auto-Switching

- **Requirement 3.1.1: Multi-Protocol Support**
  - The firmware MUST automatically detect whether the input signal is DShot or standard Servo PWM based on pulse timing in the DMA capture buffer.
  - **C Reference:** `Src/signal.c:118-200` (`transfercomplete` dispatcher).
  - **Rust Reference:** `downloads/rm32/src/transfer.rs:84` (`process` handles both), `downloads/rm32/src/signal.rs:13` (`detect_input` logic).

## 3.2 DShot Protocol

- **Requirement 3.2.1: Frame Decoding & CRC**
  - DShot frames MUST be decoded from a 16-pulse DMA sequence, verified using a 4-bit XOR checksum.
  - **C Reference:** `Src/dshot.c:70-98` (CRC and decoding).
  - **Rust Reference:** `downloads/rm32/src/dshot.rs:48-112` (`decode_frame` with CRC check).

- **Requirement 3.2.2: Bidirectional DShot (Telemetry)**
  - When enabled, the ESC MUST send a GCR-encoded telemetry frame back to the flight controller by inverting the CRC and switching the GPIO direction during the telemetry window.
  - **C Reference:** `Src/dshot.c:166-167` (Inverted CRC), `Src/dshot.c:266` (GCR encoding).
  - **Rust Reference:** `downloads/rm32/src/dshot.rs:114-162` (`encode_gcr_frame`), `downloads/rm32/src/transfer.rs:128` (Passes `dshot_telemetry` flag).

- **Requirement 3.2.3: Frame Timing Calibration**
  - The firmware MUST calibrate its internal DShot frame timing window based on the first few valid frames received while disarmed.
  - **C Reference:** `Src/signal.c:134-142` (Frame timing averaging).
  - **Rust Reference:** `downloads/rm32/src/transfer.rs:181-193` (Unarmed frame averaging).

## 3.3 Servo PWM

- **Requirement 3.3.1: Pulse Width Measurement**
  - Standard RC Servo PWM (typically 1ms to 2ms) MUST be measured via input capture interrupts and mapped to the internal throttle resolution.
  - **C Reference:** `Src/signal.c:88-100` (Servo mapping).
  - **Rust Reference:** `downloads/rm32/src/servo.rs:57-111` (Servo pulse processing).

- **Requirement 3.3.2: Stick Calibration**
  - A sequence of high and low throttle inputs while disarmed MUST trigger a stick calibration routine to set the minimum and maximum PWM pulse thresholds.
  - **C Reference:** `Src/main.c:1322-1360` (Calibration entry logic).
  - **Rust Reference:** `downloads/rm32/src/transfer.rs:195-212` (Calibration entry detection), `downloads/rm32/src/servo.rs:125-168` (`process_calibration`).
