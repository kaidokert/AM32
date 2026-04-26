# 07 Feedback & UI

This document covers user feedback mechanisms, including audible motor tones and visual LED indicators.

## 7.1 Audible Feedback (Motor Tones)

- **Requirement 7.1.1: Startup Tune**
  - The motor MUST play a multi-tone startup sequence upon power-on to indicate initialization and arming status.
  - **C Reference:** `Src/sounds.c:20-50` (`playInputTune`).
  - **Rust Reference:** `downloads/rm32/src/sounds.rs:52-64` (`play_startup`).

- **Requirement 7.1.2: BlueJay Tune Parser**
  - The firmware MUST be able to parse and play custom 128-byte melody data stored in the EEPROM, following the BlueJay-encoded format.
  - **C Reference:** `Src/sounds.c:200-250` (BlueJay tune player).
  - **Rust Reference:** `downloads/rm32/src/sounds.rs:81-125` (`play_bluejay_tune`).

- **Requirement 7.1.3: Arming & Cell Count Indication**
  - Upon arming, the motor MUST beep a specific number of times to indicate the detected battery cell count.
  - **C Reference:** `Src/main.c:1340-1342` (Cell count beep loop).
  - **Rust Reference:** `downloads/rm32_stm32/src/bin/main.rs` (Implementation of cell count beeps).

- **Requirement 7.1.4: Beacons & Error Tones**
  - The motor MUST issue repeating beacon tones after a period of inactivity and specific error tones for protection events (e.g., LVC, Stuck Rotor).
  - **C Reference:** `Src/sounds.c:260-275` (Beacon tones).
  - **Rust Reference:** `downloads/rm32/src/sounds.rs:163-178` (`play_beacon`).

## 7.2 Visual Indicators (WS2812 LED)

- **Requirement 7.2.1: Addressable LED Support**
  - The firmware MUST support driving a single WS2812/NeoPixel LED to indicate ESC status (Boot, Armed, Error).
  - **C Reference:** `Src/main.c:1335` (RGB LED init), `Src/functions.c` (LED driving).
  - **Rust Reference:** `downloads/rm32/src/ws2812.rs` (Platform-independent driver), `downloads/rm32_stm32/src/ws2812_hal.rs` (STM32 bitbang driver).

- **Requirement 7.2.2: LED Status Mapping**
  - The LED MUST change colors based on the internal state: Dim Red (Boot), Green (Armed), Red (Error).
  - **C Reference:** `Src/main.c:1112` (Error LED Red).
  - **Rust Reference:** `downloads/rm32/src/ws2812.rs:18-24` (`LedStatus` enum), `downloads/rm32_stm32/src/bin/main.rs:325-337` (Status updates).
