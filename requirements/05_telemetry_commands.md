# 05 Telemetry & Commands

This document covers telemetry reporting and DShot command processing.

## Extended DShot Telemetry (EDT)

- **REQ-TELEM-EDT_SCHEDULING:** Multiplex Current, Voltage, and Temperature into bidirectional DShot response.
  - **C Reference:** `Src/dshot.c:186-215` (`make_dshot_package` with scheduler)
  - **Rust Reference:** `downloads/rm32/src/edt.rs`, `downloads/rm32_stm32/src/isr_handlers.rs:118-128`

- **REQ-TELEM-EDT_STATE_MANAGEMENT:** Handling of EDT initialization and de-initialization sequences.
  - **C Reference:** `Src/dshot.c:262-270`
  - **Rust Reference:** `downloads/rm32/src/dshot_commands.rs`

- **REQ-TELEM-EDT_SCALING:** Telemetry MUST be scaled to DShot/KISS standards (e.g., 50mA/LSB, 25mV/LSB).
  - **C Reference:** `Src/dshot.c:203-207` (Scaling for Current/Voltage)
  - **Rust Reference:** `downloads/rm32/src/edt.rs:80-92`

- **REQ-TELEM-BIDIR_CHECKSUM:** Calculation of 4-bit inverted XOR checksum for bidirectional DShot telemetry.
  - **C Reference:** `Src/dshot.c:294-302`
  - **Rust Reference:** `downloads/rm32/src/dshot.rs`

- **REQ-TELEM-GCR_ENCODING:** Group Code Recording (GCR) encoding for bidirectional DShot physical layer.
  - **C Reference:** `Src/dshot.c:304-315`
  - **Rust Reference:** `downloads/rm32/src/dshot.rs`

- **REQ-TELEM-NRZI_ENCODING:** NRZI encoding and RLL mapping for bidirectional DShot.
  - **C Reference:** `Src/dshot.c:316-343`
  - **Rust Reference:** `downloads/rm32/src/dshot.rs`

## Telemetry Integration & Smoothing

- **REQ-TELEM-CURRENT_SMOOTHING:** Noise filtering for ADC current readings via moving average.
  - **C Reference:** `Src/main.c:390-403` (`getSmoothedCurrent`)
  - **Rust Reference:** `downloads/rm32_stm32/src/main_loop.rs`

- **REQ-TELEM-CURRENT_INTEGRATION:** Integration of instantaneous current to calculate total consumed mAh.
  - **C Reference:** `Src/main.c:1035-1038`
  - **Rust Reference:** `downloads/rm32_stm32/src/main_loop.rs`

## DShot Commands

- **REQ-TELEM-DSHOT_CMD:** Process DShot commands 1-47 (direction, beeps, saves, etc.).
  - **C Reference:** `Src/dshot.c:217-300` (Command switch-case)
  - **Rust Reference:** `downloads/rm32/src/dshot_commands.rs`

- **REQ-TELEM-PROG_MODE:** Multi-step "Programming Mode" (cmd 36) for interactive EEPROM modification.
  - **C Reference:** `Src/dshot.c:110-128` (Programming mode state machine)
  - **Rust Reference:** `downloads/rm32/src/dshot_commands.rs:125-155`

- **REQ-TELEM-INFO_PACKET:** Packaging of configuration data with CRC for telemetry transmission.
  - **C Reference:** `Src/kiss_telemetry.c:35-41` (`makeInfoPacket`)
  - **Rust Reference:** `downloads/rm32/src/telemetry.rs`
