# 06 Configuration & EEPROM

This document details persistent configuration, storage, and version management.

## EEPROM Layout & Storage

- **REQ-CFG-LAYOUT:** Standardized 192-byte `EEprom_t` (C) / `EepromConfig` (Rust) structure.
  - **C Reference:** `Inc/eeprom.h:20-65` (EEPROM struct)
  - **Rust Reference:** `downloads/rm32/src/config.rs:17-68` (`EepromConfig` struct)

- **REQ-CFG-FLASH_STORAGE:** Settings MUST be stored in dedicated MCU flash sector.
  - **C Reference:** `Src/main.c:202` (`eeprom_address` calculation)
  - **Rust Reference:** `downloads/rm32_stm32/src/flash.rs`, `downloads/rm32_stm32/src/bin/main.rs:149-163` (Dynamic address calculation)

- **REQ-CFG-SAVE_TO_FLASH:** Core function for committing current configuration to persistent flash memory.
  - **C Reference:** `Src/main.c:375-388` (`saveEEpromSettings`)
  - **Rust Reference:** `downloads/rm32_stm32/src/main_loop.rs`

## Versioning & Migration

- **REQ-CFG-VERSIONING:** Apply safe defaults if stored version is older than firmware version.
  - **C Reference:** `Src/main.c:595-620` (`loadEEpromSettings` migration logic)
  - **Rust Reference:** `downloads/rm32/src/config.rs:114-129` (`apply_version_defaults`)

- **REQ-CFG-INTEGRITY_CHECK:** Detect blank/corrupt flash (0xFF) and fall back to hardcoded defaults.
  - **C Reference:** `Src/main.c:1358` (implicit check)
  - **Rust Reference:** `downloads/rm32/src/config.rs:107-111` (`is_valid` check for `0xFF`)

## Dynamic Scaling & Calibration

- **REQ-CFG-SERVO_SCALING:** Scale raw EEPROM bytes for servo thresholds to micro-seconds.
  - **C Reference:** `Src/main.c:681-683` (Servo threshold scaling)
  - **Rust Reference:** `downloads/rm32_stm32/src/bin/main.rs` (Implementation of servo parameter propagation)

- **REQ-CFG-DEAD_TIME:** Dynamically adjust PWM dead-time based on `driving_brake_strength`.
  - **C Reference:** `Src/main.c:691-710` (Dead-time calculation and BDTR write)
  - **Rust Reference:** `downloads/rm32_stm32/src/bin/main.rs:225-252` (Dead-time override application)

- **REQ-CFG-KV_SCALING:** Motor KV division based on battery cell count variants (e.g., divide by 16 for `ONE_TWO_CELL_MAX`).
  - **C Reference:** `Src/main.c:650-655` (KV scaling)
  - **Rust Reference:** `downloads/rm32_stm32/src/bin/main.rs:185`
