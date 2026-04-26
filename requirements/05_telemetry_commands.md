# 05 Telemetry & Commands

This document covers the communication of ESC data back to the controller and the processing of special DShot commands.

## 5.1 Extended DShot Telemetry (EDT)

- **Requirement 5.1.1: EDT Scheduling**
  - Extended telemetry (Current, Voltage, Temperature) MUST be multiplexed into the bidirectional DShot response, alternating with eRPM frames at configured rates.
  - **C Reference:** `Src/dshot.c:186-215` (`make_dshot_package` with scheduler).
  - **Rust Reference:** `downloads/rm32/src/edt.rs` (`EdtScheduler` logic), `downloads/rm32_stm32/src/isr_handlers.rs:118-128`.

- **Requirement 5.1.2: Telemetry Data Scaling**
  - Telemetry data MUST be scaled according to the DShot/KISS standards (e.g., 50mA per LSB for current, 25mV per LSB for voltage).
  - **C Reference:** `Src/dshot.c:203-207` (Scaling for Current/Voltage).
  - **Rust Reference:** `downloads/rm32/src/edt.rs:80-92` (EDT scaling logic).

## 5.2 DShot Commands (1-47)

- **Requirement 5.2.1: Special Command Processing**
  - The firmware MUST process DShot commands (values 1-47) received over the signal line, including direction changes, beep requests, and settings saves.
  - **C Reference:** `Src/dshot.c:217-300` (Command switch-case).
  - **Rust Reference:** `downloads/rm32/src/dshot_commands.rs` (`CommandProcessor` and `process` method).

## 5.3 Programming Mode

- **Requirement 5.3.1: Interactive EEPROM Modification**
  - The firmware MUST support a multi-step "Programming Mode" (triggered by DShot command 36) that allows the controller to write specific bytes to the ESC's EEPROM/Flash.
  - **C Reference:** `Src/dshot.c:110-128` (Programming mode state machine).
  - **Rust Reference:** `downloads/rm32/src/dshot_commands.rs:125-155` (`process_programming`).
