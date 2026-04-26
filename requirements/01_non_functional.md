# 01 Non-Functional Requirements

This document outlines the hardware, performance, and resource constraints for the AM32 firmware.

## 1.1 Target Hardware Support
The firmware MUST support a variety of ARM Cortex-M based microcontrollers, specifically maintaining clean abstractions to port across different families.

- **Requirement 1.1.1: STM32F0 Series**
  - **C Reference:** `Inc/targets.h` (defines like `MCU_F051`), `Mcu/f051/`
  - **Rust Reference:** `downloads/rm32_stm32/src/interrupts_f051.rs`, `downloads/rm32_stm32/src/adc_f051.rs`

- **Requirement 1.1.2: STM32G0 Series**
  - **C Reference:** `Inc/targets.h` (defines like `MCU_G071`), `Mcu/g071/`
  - **Rust Reference:** `downloads/rm32_stm32/src/interrupts_g071.rs`, `downloads/rm32_stm32/src/comp_init.rs`

- **Requirement 1.1.3: STM32L4 Series**
  - **C Reference:** `Inc/targets.h` (defines like `MCU_L431`), `Mcu/l431/`
  - **Rust Reference:** `downloads/rm32_stm32/src/interrupts_l431.rs`, `downloads/rm32_stm32/src/adc_l431.rs`

## 1.2 Resource Constraints

- **Requirement 1.2.1: Flash Footprint**
  - The compiled firmware MUST fit within the target MCU's flash memory with approximately 20% spare capacity to allow for future feature expansion.
  - **C Context:** Typically < 32KB.
  - **Rust Context:** `rm32_stm32` release binary for G071 is ~7.4KB (referenced in `downloads/rm32/TEST_PARITY.md`).

- **Requirement 1.2.2: Deterministic Timing (20kHz Tick)**
  - The core control loop MUST execute at a deterministic frequency, typically 20kHz, to ensure smooth motor commutation and responsive PID control.
  - **C Reference:** `Src/main.c:1385` (implied by `tenkhzcounter` logic).
  - **Rust Reference:** `downloads/rm32_stm32/src/isr_handlers.rs:57` (calls `ten_khz_tick`).

## 1.3 Safety and Robustness

- **Requirement 1.3.1: Hardware Watchdog**
  - An Independent Watchdog (IWDG) MUST be active to reset the MCU in the event of a software hang.
  - **C Reference:** `Src/main.c:1316` (`RELOAD_WATCHDOG_COUNTER()`).
  - **Rust Reference:** `downloads/rm32_stm32/src/bin/main.rs:275` (Watchdog started), `downloads/rm32_stm32/src/system.rs` (HAL Feed).
