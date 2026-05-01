# MCU Requirement Discovery Report

This report identifies contiguous blocks of code in chip-specific directories (`Mcu/f051/Src/`, `Mcu/g071/Src/`, and `Mcu/l431/Src/`) that lack `/* REQTRK: SLUG */` comments.

## 1. Comparator Logic (`comparator.c`)

| MCU | Block (Lines) | Logic | Proposed Requirement / Status |
|---|---|---|---|
| f051 | 13-13 | `getCompOutputLevel` | `REQ-HW-COMP-READ`: Read comparator output level. |
| f051 | 15-19 | `maskPhaseInterrupts` | `REQ-HW-COMP-INT_MASK`: Mask BEMF zero-crossing interrupts. |
| f051 | 21-21 | `enableCompInterrupts` | `REQ-HW-COMP-INT_ENABLE`: Enable BEMF zero-crossing interrupts. |
| f051 | 23-33 | `changeCompInput` | `REQ-HW-COMP-CFG`: Dynamic comparator hysteresis/filtering and EXTI polarity adjustment. |
| g071 | 15-15 | `getCompOutputLevel` | `REQ-HW-COMP-READ`: Direct register access for comparator output. |
| g071 | 17-29 | `maskPhaseInterrupts` | `REQ-HW-COMP-INT_MASK`: Multi-line EXTI mask (supports N-variant). |
| g071 | 33-59 | `changeCompInput` | `REQ-HW-COMP-PWR`: Speed-based power mode switching (High vs Medium speed). |
| g071 | 63-125 | Commented out code | **DEAD CODE**: Redundant implementations of `changeCompInput`. |
| l431 | 11-11 | `getCompOutputLevel` | `REQ-HW-COMP-READ`: LL-based comparator read. |
| l431 | 22-38 | `changeCompInput` | `REQ-HW-COMP-INPUT_SEL`: Commutation step-based input selection. |

## 2. Phase Control (`phaseouts.c`)

| MCU | Block (Lines) | Logic | Proposed Requirement / Status |
|---|---|---|---|
| All | 43-65 | `proportionalBrake` | `REQ-HW-PHASE-BRAKE_PROP`: PWM-based braking using low-side MOSFETs. |
| All | 67-151 | `phase[A/B/C][PWM/FLOAT/LOW]` | `REQ-HW-PHASE-STATE_CTRL`: Individual MOSFET state control (6-step compatible). |
| All | 269-273 | `allOff` | `REQ-HW-PHASE-ALL_OFF`: Simultaneous floating of all phases (Coasting). |
| All | 275-317 | `comStep` | `REQ-HW-PHASE-COMMUTATION`: Hardware-level 6-step commutation sequence. |
| All | 319-324 | `fullBrake` | `REQ-HW-PHASE-BRAKE_FULL`: Shorting all low-sides for maximum braking torque. |

## 3. Peripheral Initialization (`peripherals.c`)

| MCU | Block (Lines) | Logic | Proposed Requirement / Status |
|---|---|---|---|
| f051 | 73-100 | `SystemClock_Config` | `REQ-HW-CLK-CFG`: HSI/PLL configuration for 48MHz operation. |
| f051 | 102-124 | `MX_COMP1_Init` | `REQ-HW-COMP-INIT`: Hardware comparator and associated GPIO/NVIC setup. |
| f051 | 126-133 | `MX_IWDG_Init` | `REQ-HW-WATCHDOG-CFG`: Independent Watchdog setup. |
| f051 | 135-245 | `MX_TIM1_Init` | `REQ-HW-PWM-MAIN_CFG`: Timer 1 configuration for 3-phase PWM with dead-time. |
| f051 | 358-408 | `UN_TIM_Init` | `REQ-HW-INPUT-CAPTURE_CFG`: Input capture timer for control signals. |
| f051 | 555-659 | `enableCorePeripherals` | `REQ-HW-BOOT-SEQUENCE`: Atomic start of all critical motor control peripherals. |

## 4. Signal I/O (`IO.c`)

| MCU | Block (Lines) | Logic | Proposed Requirement / Status |
|---|---|---|---|
| f051 | 21-50 | `receiveDshotDma` | `REQ-HW-DSHOT-RX_DMA`: DMA-backed DShot input capture. |
| f051 | 52-82 | `sendDshotDma` | `REQ-HW-DSHOT-TX_DMA`: DMA-backed DShot output telemetry. |
| f051 | 86-101 | Input Pin Control | `REQ-HW-INPUT-PIN_CTRL`: Pull-up/down and polarity management for signal pin. |

## 5. Flash/EEPROM Logic (`eeprom.c`)

| MCU | Block (Lines) | Logic | Proposed Requirement / Status |
|---|---|---|---|
| f051 | 20-65 | `save_flash_nolib` | `REQ-HW-FLASH-WRITE_16BIT`: 16-bit word writing to flash. |
| g071 | 16-63 | `save_flash_nolib` | `REQ-HW-FLASH-WRITE_64BIT`: 64-bit double-word writing for G071. |
| l431 | 22-79 | `save_flash_nolib` | `REQ-HW-FLASH-WRITE_ALIGN`: 8-byte aligned writing with error clearing. |

## 6. Analog to Digital Conversion (`ADC.c`)

| MCU | Block (Lines) | Logic | Proposed Requirement / Status |
|---|---|---|---|
| All | 21-41 | `ADC_DMA_Callback` | `REQ-HW-ADC-DATA_MAPPING`: Buffer to parameter mapping (Volt/Curr/Temp). |
| All | 43-69 | `enableADC_DMA` | `REQ-HW-ADC-DMA_CIRC`: Circular DMA configuration for continuous sampling. |
| All | 71-93 | `activateADC` | `REQ-HW-ADC-CALIBRATION`: Self-calibration and regulator stabilization. |
| f051 | 95-154 | `ADC_Init` | `REQ-HW-ADC-SEQUENCER`: Channel sequence and sampling time setup. |

## 7. Serial Telemetry (`serial_telemetry.c`)

| MCU | Block (Lines) | Logic | Proposed Requirement / Status |
|---|---|---|---|
| f051 | 13-68 | `telem_UART_Init` | `REQ-HW-TELEM-UART_CFG`: USART configuration for 115200 baud telemetry. |
| f051 | 70-78 | `send_telem_DMA` | `REQ-HW-TELEM-DMA_TX`: Non-blocking telemetry transmission using DMA. |

## Summary of Potential Dead Code
- **Mcu/g071/Src/comparator.c**: Large blocks of commented-out `changeCompInput` variants (Lines 63-125).
- **Mcu/f051/Src/peripherals.c**: Commented-out NVIC configurations and timer initializations (Lines 237, 269, 313, 381-400).
- **Mcu/l431/Src/ADC.c**: Commented out `marker` variable and IRQ setup (Lines 28, 62).
