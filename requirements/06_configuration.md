# 06 Configuration & EEPROM

This document details the persistent configuration layout, storage, and version management.

## 6.1 EEPROM Layout & Storage

- **Requirement 6.1.1: Standardized Config Struct**
  - ESC settings MUST be stored in a 192-byte `EEprom_t` (C) / `EepromConfig` (Rust) structure.
  - **C Reference:** `Inc/eeprom.h:20-65` (EEPROM struct).
  - **Rust Reference:** `downloads/rm32/src/config.rs:17-68` (`EepromConfig` struct).

- **Requirement 6.1.2: Flash-Based Emulation**
  - Persistent settings MUST be stored in a dedicated sector of the MCU's flash memory, typically at the end of the available flash space.
  - **C Reference:** `Src/main.c:202` (`eeprom_address` calculation).
  - **Rust Reference:** `downloads/rm32_stm32/src/flash.rs` (HAL driver), `downloads/rm32_stm32/src/bin/main.rs:149-163` (Dynamic address calculation).

## 6.2 Versioning & Migration

- **Requirement 6.2.1: Version Migration**
  - Upon loading, the firmware MUST verify the EEPROM version and apply safe defaults if the stored version is older than the current firmware's expected version.
  - **C Reference:** `Src/main.c:595-620` (`loadEEpromSettings` migration logic).
  - **Rust Reference:** `downloads/rm32/src/config.rs:114-129` (`apply_version_defaults`).

- **Requirement 6.2.2: Flash Integrity Check**
  - The firmware MUST detect blank or corrupt flash (e.g., all `0xFF`) and fall back to hardcoded safe defaults.
  - **C Reference:** `Src/main.c:1358` (implicit check).
  - **Rust Reference:** `downloads/rm32/src/config.rs:107-111` (`is_valid` check for `0xFF`).

## 6.3 Dynamic Scaling & Calibration

- **Requirement 6.3.1: Servo Threshold Scaling**
  - Raw EEPROM bytes for servo thresholds MUST be scaled to micro-seconds: `threshold = (byte * 2) + offset`.
  - **C Reference:** `Src/main.c:681-683` (Servo threshold scaling).
  - **Rust Reference:** `downloads/rm32_stm32/src/bin/main.rs` (Implementation of servo parameter propagation).

- **Requirement 6.3.2: Dead-Time Override**
  - The hardware PWM dead-time MUST be dynamically adjusted based on the configured `driving_brake_strength` to ensure FET safety and braking performance.
  - **C Reference:** `Src/main.c:691-710` (Dead-time calculation and BDTR write).
  - **Rust Reference:** `downloads/rm32_stm32/src/bin/main.rs:225-252` (Dead-time override application).

- **Requirement 6.3.3: Motor KV Scaling**
  - The firmware MUST scale the configured motor KV based on the battery cell count variant (e.g., divide by 2 for `THREE_CELL_MAX`, divide by 16 for `ONE_TWO_CELL_MAX`).
  - **C Reference:** `Src/main.c:650-655` (KV scaling).
  - **Rust Reference:** Not explicitly implemented as compile-time variants, but `motor_kv` scaling exists in `main.rs`.
