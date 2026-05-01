# 11 Bootloader Interface

This document details the shared interfaces and compatibility requirements between the AM32 bootloader and the firmware.

## Bootloader Persistence & Jump

- **REQ-HW-BOOTLOADER_JUMP:** The firmware MUST be linked to start at a specific offset defined by the bootloader (typically `0x1000` for STM32F0/G0 or `0x4000` for STM32F4).
  - **C Reference:** `Src/main.c:1713` (entry point after jump), `Mcu/f051/Inc/peripherals.h` (`APPLICATION_ADDRESS`).
  - **Bootloader Reference:** `bootloader/main.c:286` (`jump()` function).

- **REQ-STATE-BOOTLOADER_REBOOT:** The firmware MUST be able to return to the bootloader by triggering a software reset. The bootloader detects the software reset flag and enters serial command mode instead of jumping back to the application.
  - **C Reference:** `Src/main.c:1501` (`NVIC_SystemReset()`).
  - **Bootloader Reference:** `bootloader/main.c:920` (`bl_was_software_reset()` check).

## Hardware Initialization & Remapping

- **REQ-HW-VECTOR_REMAP:** On MCUs without a Vector Table Offset Register (VTOR), such as Cortex-M0, the firmware MUST copy its vector table to the start of SRAM (`0x20000000`) and use the `SYSCFG` remap register to map SRAM to address `0x00000000`.
  - **C Reference:** `Mcu/f051/Src/peripherals.c:38-59` (`initAfterJump`).
  - **Rust Reference:** `downloads/rm32_stm32/src/bin/main.rs:135` (Copying vectors).

## Shared Data & Configuration

- **REQ-CFG-BOOTLOADER_INFO:** The firmware MUST check for a "Device Info" structure located in the last 32 bytes of the bootloader flash area. This structure contains magic numbers and a flash size code used to dynamically calculate the EEPROM address.
  - **C Reference:** `Src/main.c:1681-1704` (`checkDeviceInfo`).
  - **Rust Reference:** `downloads/rm32_stm32/src/bin/main.rs:149-163`.
  - **Bootloader Reference:** `bootloader/main.c:186-198` (`DEVINFO` struct).

- **REQ-CFG-BOOTLOADER_VERSION:** The firmware SHOULD reserve the 3rd byte of the EEPROM (`eeprom[2]`) for the bootloader's version identifier. The bootloader automatically updates this byte if it differs from its internal version.
  - **Bootloader Reference:** `bootloader/main.c:854-884` (`update_EEPROM`).
