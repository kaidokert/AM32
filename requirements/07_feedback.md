# 07 Feedback & UI

This document covers audible tones and visual LED indicators.

## Audible Feedback (Motor Tones)

- **REQ-UI-STARTUP_TUNE:** Multi-tone startup sequence upon power-on.
  - **C Reference:** `Src/sounds.c:108` (`playStartupTune`)
  - **Rust Reference:** `downloads/rm32/src/sounds.rs:52-64` (`play_startup`)

- **REQ-UI-BLUEJAY_TUNE:** Parse and play 128-byte melody data in BlueJay format.
  - **C Reference:** `Src/sounds.c:63` (`playBlueJayTune`)
  - **Rust Reference:** `downloads/rm32/src/sounds.rs:81-125` (`play_bluejay_tune`)

- **REQ-UI-BLUEJAY_UTIL:** Utilities for translating BlueJay data into frequencies and durations.
  - **C Reference:** `Src/sounds.c:46-60` (`playBJNote`, `getBlueJayNoteFrequency`)
  - **Rust Reference:** `downloads/rm32/src/sounds.rs`

- **REQ-UI-CELL_COUNT_BEEPS:** Beep battery cell count upon arming.
  - **C Reference:** `Src/main.c:1340-1342`, `Src/sounds.c:209` (`playInputTune`)
  - **Rust Reference:** `downloads/rm32_stm32/src/bin/main.rs`

- **REQ-UI-BEACON_ERROR_TONES:** Repeating beacon tones and protection error tones.
  - **C Reference:** `Src/sounds.c:261` (`playBeaconTune3`)
  - **Rust Reference:** `downloads/rm32/src/sounds.rs:163-178` (`play_beacon`)

- **REQ-UI-BRUSHED_STARTUP:** Specific startup melody for brushed motor configurations.
  - **C Reference:** `Src/sounds.c:115-134` (`playBrushedStartupTune`)
  - **Rust Reference:** `downloads/rm32/src/sounds.rs`

- **REQ-UI-DUSKING_TUNE:** Specific "Dusking" melody pattern for state feedback.
  - **C Reference:** `Src/sounds.c:136-159` (`playDuskingTune`)
  - **Rust Reference:** `downloads/rm32/src/sounds.rs`

- **REQ-UI-INPUT_FEEDBACK:** Auditory feedback patterns acknowledging user input changes.
  - **C Reference:** `Src/sounds.c:161-180` (`playInputTune2`)
  - **Rust Reference:** `downloads/rm32/src/sounds.rs`

- **REQ-UI-CONFIG_FEEDBACK:** Auditory patterns for configuration state changes (Default vs Changed).
  - **C Reference:** `Src/sounds.c:199-228` (`playDefaultTone`, `playChangedTone`)
  - **Rust Reference:** `downloads/rm32/src/sounds.rs`

- **REQ-UI-AUDIO_CFG:** Configuration of beep volume and basic audio timing.
  - **C Reference:** `Src/sounds.c:22-44` (`pause`, `setVolume`, `setCaptureCompare`)
  - **Rust Reference:** `downloads/rm32/src/sounds.rs`

## Visual Indicators (WS2812 LED)

- **REQ-UI-LED_WS2812:** Support for single WS2812 status LED.
  - **C Reference:** `Src/main.c:1335`, `Src/functions.c`
  - **Rust Reference:** `downloads/rm32/src/ws2812.rs`, `downloads/rm32_stm32/src/ws2812_hal.rs`

- **REQ-UI-LED_MAPPING:** Map internal state to LED colors (Boot, Armed, Error).
  - **C Reference:** `Src/main.c:1112`
  - **Rust Reference:** `downloads/rm32/src/ws2812.rs:18-24`, `downloads/rm32_stm32/src/bin/main.rs:325-337`

- **REQ-UI-CUSTOM_LED:** Board-specific visual feedback based on throttle position.
  - **C Reference:** `Src/main.c:1015-1033` (`#ifdef USE_CUSTOM_LED`)
  - **Rust Reference:** `downloads/rm32_stm32/src/main_loop.rs` (PB3 logic)
