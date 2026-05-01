# 01 Non-Functional Requirements

This document outlines the hardware, performance, and resource constraints for the AM32 firmware.

## Hardware Support

- **REQ-HW-STM32F0:** Support for STM32F0 series microcontrollers.
  - **C Reference:** `Inc/targets.h` (defines like `MCU_F051`), `Mcu/f051/`
  - **Rust Reference:** `downloads/rm32_stm32/src/interrupts_f051.rs`, `downloads/rm32_stm32/src/adc_f051.rs`

- **REQ-HW-STM32G0:** Support for STM32G0 series microcontrollers.
  - **C Reference:** `Inc/targets.h` (defines like `MCU_G071`), `Mcu/g071/`
  - **Rust Reference:** `downloads/rm32_stm32/src/interrupts_g071.rs`, `downloads/rm32_stm32/src/comp_init.rs`

- **REQ-HW-STM32L4:** Support for STM32L4 series microcontrollers.
  - **C Reference:** `Inc/targets.h` (defines like `MCU_L431`), `Mcu/l431/`
  - **Rust Reference:** `downloads/rm32_stm32/src/interrupts_l431.rs`, `downloads/rm32_stm32/src/adc_l431.rs`

- **REQ-HW-STM32G4:** Support for STM32G4 series microcontrollers.
  - **C Reference:** `Inc/targets.h` (defines like `MCU_G431`), `Mcu/g431/`
  - **Rust Reference:** `downloads/rm32_stm32/src/adc_g431.rs`, `downloads/rm32_stm32/src/comp_init_g431.rs`, `downloads/rm32_stm32/src/interrupts_g431.rs`

- **REQ-HW-EXTERNAL_NTC:** Support for external NTC thermistors via look-up tables.
  - **C Reference:** `Src/main.c:2045`, `Inc/ntc_tables.h`
  - **Rust Reference:** `downloads/rm32/src/ntc.rs`, `downloads/rm32_stm32/src/main_loop.rs:150` (branch logic)

- **REQ-HW-DUAL_ADC:** Triggering conversions on a secondary ADC unit.
  - **C Reference:** `Src/main.c:2030` (`#ifdef USE_ADC_1_2`)
  - **Rust Reference:** `downloads/rm32_stm32/src/adc_g431.rs` (`G431DualAdc`), `main_loop.rs` (dual trigger)

- **REQ-HW-MULTI_COMPARATOR:** Support for switching between multiple hardware comparators per commutation step.
  - **C Reference:** `Mcu/g431/Src/comparator.c`
  - **Rust Reference:** `downloads/rm32_stm32/src/comparator_g431.rs` (Dual COMP1+COMP2 switching)

- **REQ-HW-TIMING:** Precise microsecond and millisecond hardware delays.
  - **C Reference:** `Src/functions.c` (`delayMicros`, `delayMillis`)
  - **Rust Reference:** `downloads/rm32_stm32/src/system.rs`

- **REQ-HW-TIMER_READ:** High-resolution 16-bit microsecond timer reading.
  - **C Reference:** `Src/functions.c:43-56` (`get_timer_us16`)
  - **Rust Reference:** `downloads/rm32_stm32/src/timer.rs`

- **REQ-HW-GPIO_CONFIG:** Low-level hardware-specific GPIO configuration helpers.
  - **C Reference:** `Src/functions.c:98-129` (`gpio_mode_QUICK`, `gpio_mode_set`)
  - **Rust Reference:** `downloads/rm32_stm32/src/gpio.rs`

- **REQ-HW-ADC_PROCESSING:** Conversion of raw ADC counts to physical units.
  - **C Reference:** `Src/main.c:1110-1153`
  - **Rust Reference:** `downloads/rm32_stm32/src/main_loop.rs:144-150`

## Resource Constraints

- **REQ-RES-FLASH_FOOTPRINT:** The compiled firmware MUST fit within target MCU flash with ~20% spare capacity.
  - **C Context:** Typically < 32KB.
  - **Rust Context:** `rm32_stm32` release binary for G071 is ~7.4KB.

- **REQ-RES-TICK_RATE:** The core control loop MUST execute at a deterministic frequency, typically 20kHz.
  - **C Reference:** `Src/main.c:1385`
  - **Rust Reference:** `downloads/rm32_stm32/src/isr_handlers.rs:57`

- **REQ-RES-IRQ_PRIORITY_DYNAMIC:** Dynamic interrupt priority management based on motor RPM.
  - **C Reference:** `Src/main.c:1084-1096`
  - **Rust Reference:** `downloads/rm32_stm32/src/bin/main.rs:330`

## Safety and Robustness

- **REQ-SAFE-WATCHDOG:** An Independent Watchdog (IWDG) MUST be active to reset the MCU on software hang.
  - **C Reference:** `Src/main.c:1316` (`RELOAD_WATCHDOG_COUNTER()`)
  - **Rust Reference:** `downloads/rm32_stm32/src/bin/main.rs:275`, `downloads/rm32_stm32/src/system.rs`

- **REQ-SAFE-SIGNAL_TIMEOUT:** Safe ESC reset upon prolonged control signal loss.
  - **C Reference:** `Src/main.c:986-1014`
  - **Rust Reference:** `downloads/rm32/src/control/tick.rs`

## Utilities

- **REQ-UTIL-MATH:** Common mathematical functions such as linear mapping and absolute difference.
  - **C Reference:** `Src/functions.c` (`map`, `getAbsDif`)
  - **Rust Reference:** `downloads/rm32/src/functions.rs`

- **REQ-UTIL-CRC8:** Standard CRC8 calculation utility for data integrity.
  - **C Reference:** `Src/functions.c:78-96` (`update_crc8`, `get_crc8`)
  - **Rust Reference:** `downloads/rm32/src/functions.rs`
