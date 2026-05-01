# Requirements Discovery: Source Files Audit

This document identifies code blocks in core source files that are not currently tracked by a `/* REQTRK: SLUG */` comment and proposes corresponding requirements or identifies them as dead code.

## 1. Src/kiss_telemetry.c

| Lines | Logic Summary | Proposed Requirement / Status |
|-------|---------------|-------------------------------|
| 35-41 | `makeInfoPacket`: Packages 48 bytes of EEPROM buffer with a CRC8 for telemetry. | `REQ-TELEM-INFO_PACKET`: The system shall be able to package ESC configuration information with CRC protection for telemetry transmission. |

## 2. Src/functions.c

| Lines | Logic Summary | Proposed Requirement / Status |
|-------|---------------|-------------------------------|
| 43-56 | `get_timer_us16`: Hardware-specific abstraction for reading the 16-bit microsecond utility timer. | `REQ-HW-TIMER_READ`: The system shall provide an abstraction to read the current value of the high-resolution utility timer in microseconds. |
| 78-96 | `update_crc8` and `get_crc8`: Standard CRC8 calculation utilities. | `REQ-UTIL-CRC8`: The system shall provide a CRC8 calculation utility for data integrity verification. |
| 98-129 | `gpio_mode_QUICK` and `gpio_mode_set`: Low-level GPIO configuration for specific MCUs (AT421, AT415). | `REQ-HW-GPIO_CONFIG`: The system shall provide low-level functions to quickly configure GPIO modes and pull-up/pull-down settings. |

## 3. Src/sounds.c

| Lines | Logic Summary | Proposed Requirement / Status |
|-------|---------------|-------------------------------|
| 22-44 | Audio utilities: `pause`, `setVolume`, `setCaptureCompare` for PWM duty cycle management. | `REQ-UI-AUDIO_CFG`: The system shall provide functions to configure beep volume and basic audio timing. |
| 46-60 | BlueJay helpers: `playBJNote` and `getBlueJayNoteFrequency` for translating BlueJay tune data. | `REQ-UI-BLUEJAY_UTIL`: The system shall provide utilities to translate BlueJay tune data into specific PWM frequencies and durations. |
| 115-134 | `playBrushedStartupTune`: Startup melody specifically for brushed motor configurations. | `REQ-UI-BRUSHED_STARTUP`: The system shall provide a specific startup melody for brushed motor configurations. |
| 136-159 | `playDuskingTune`: A specific melody pattern for state feedback. | `REQ-UI-DUSKING_TUNE`: The system shall provide a "Dusking" melody for state feedback. |
| 161-180 | `playInputTune2`: Secondary auditory feedback pattern for user input. | `REQ-UI-INPUT_FEEDBACK`: The system shall provide auditory feedback patterns for user input acknowledgement. |
| 199-228 | `playDefaultTone` and `playChangedTone`: Feedback for configuration state changes. | `REQ-UI-CONFIG_FEEDBACK`: The system shall provide distinct auditory signals to indicate default vs changed configuration states. |

## 4. Src/signal.c

| Lines | Logic Summary | Proposed Requirement / Status |
|-------|---------------|-------------------------------|
| 173-237 | `checkDshot`, `checkServo`, and `detectInput`: Logic for automatic input protocol detection based on pulse timing. | `REQ-SIG-INPUT_DETECTION`: The system shall automatically detect and configure the input signal type (DShot or Servo PWM) based on timing analysis of the incoming signal. |

## 5. Src/dshot.c

| Lines | Logic Summary | Proposed Requirement / Status |
|-------|---------------|-------------------------------|
| 123-157 | Extraction of throttle values and commands from DShot frames, including DroneCAN override logic. | `REQ-SIG-DSHOT_INPUT_PROCESSING`: The system shall process decoded DShot frames to extract throttle values, commands, or handle idle states. |
| 262-270 | Initialization and de-initialization of Extended DShot Telemetry (EDT) frames. | `REQ-TELEM-EDT_STATE_MANAGEMENT`: The system shall handle initialization and de-initialization of Extended DShot Telemetry (EDT). |
| 294-302 | Bidirectional DShot checksum calculation (inverted 4-bit XOR). | `REQ-TELEM-BIDIR_CHECKSUM`: The system shall calculate a 4-bit inverted XOR checksum for bidirectional DShot telemetry frames. |
| 304-315 | GCR (Group Code Recording) encoding (16-bit to 20-bit). | `REQ-TELEM-GCR_ENCODING`: The system shall encode 16-bit DShot data into 20-bit GCR format for telemetry transmission. |
| 316-343 | NRZI encoding and RLL mapping for the bidirectional DShot physical layer. | `REQ-TELEM-NRZI_ENCODING`: The system shall apply NRZI encoding to the GCR data for the bidirectional DShot physical layer. |

## 6. Src/firmwareversion.c

| Lines | Logic Summary | Proposed Requirement / Status |
|-------|---------------|-------------------------------|
| N/A | Entire file is tracked by `REQ-CFG-VERSIONING`. | Fully Covered. |
