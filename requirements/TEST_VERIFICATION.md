# Requirements Test Verification Matrix

This document tracks the verification status of all active requirements for the AM32 firmware. Coverage is provided through high-fidelity blackbox test vectors and Rust unit tests.

| Requirement ID | Description | Blackbox Vector | Unit Test | Status |
|:---|:---|:---|:---|:---|
| **Hardware / Build** | | | | |
| REQ-HW-STM32F0 | Support for STM32F0 series | - | - | [BUILD] |
| REQ-HW-STM32G0 | Support for STM32G0 series | - | - | [BUILD] |
| REQ-HW-STM32G4 | Support for STM32G4 series | - | - | [BUILD] |
| REQ-HW-STM32L4 | Support for STM32L4 series | - | - | [BUILD] |
| REQ-HW-EXTERNAL_NTC | External NTC thermistor support | - | ntc.rs:ntc_midrange | [OK] |
| REQ-HW-TIMING | Precise hardware delays | - | - | [BUILD] |
| REQ-HW-TIMER_READ | 16-bit microsecond timer | - | - | [BUILD] |
| REQ-HW-DUAL_ADC | Secondary ADC unit support | - | - | [BUILD] |
| REQ-HW-MULTI_COMPARATOR | Multi-comparator switching | - | - | [BUILD] |
| **Safety & Utilities** | | | | |
| REQ-SAFE-WATCHDOG | Independent Watchdog (IWDG) | - | - | [BUILD] |
| REQ-SAFE-SIGNAL_TIMEOUT | Safe reset on signal loss | signal_timeout.txt | - | [OK] |
| REQ-UTIL-MATH | Math helpers (map, abs) | - | functions.rs:test_map_basic | [OK] |
| REQ-UTIL-CRC8 | CRC8 calculation utility | - | functions.rs:test_crc8 | [OK] |
| **Control Loop & State** | | | | |
| REQ-STATE-ARMED | Motor spins only when Armed | arming_basic.txt | - | [OK] |
| REQ-STATE-RUNNING | Running state entry/exit | motor_commutation.txt | control/tests.rs:start_motor_when_stopped | [OK] |
| REQ-PROT-LVC | Low Voltage Cutoff | low_voltage_cutoff.txt | - | [OK] |
| REQ-PROT-STUCK_ROTOR | Stuck rotor protection | bemf_timeout.txt | - | [OK] |
| REQ-PROT-DESYNC_DETECTION | Desync detection | desync_detection.txt | - | [OK] |
| REQ-PROT-TEMP_LIMIT | Temperature limiting | temperature_limit.txt | - | [OK] |
| REQ-PROT-RC_CAR_REVERSE | RC Car bidirectional control | rc_car_reverse.txt | - | [OK] |
| **Signal & Telemetry** | | | | |
| REQ-SIG-AUTO_DETECT | Auto protocol detection | servo_input.txt | - | [OK] |
| REQ-SIG-DSHOT_DECODE | DShot frame decoding | dshot_input.txt | dshot.rs:decode_throttle | [OK] |
| REQ-SIG-INVERTED_EXTI | Inverted signal support | bidir_dshot.txt | - | [OK] |
| REQ-TELEM-EDT_SCHEDULING | EDT multiplexing | dshot_commands_extended.txt | - | [OK] |
| REQ-TELEM-BIDIR_CHECKSUM | Bidir DShot checksum | - | dshot.rs:decode_bad_crc | [OK] |
| REQ-TELEM-GCR_ENCODING | GCR encoding for Bidir | - | - | [MISSING] |
| REQ-TELEM-NRZI_ENCODING | NRZI encoding for Bidir | - | - | [MISSING] |
| REQ-TELEM-INFO_PACKET | Config info packet | telemetry.txt | - | [OK] |
| REQ-TELEM-DSHOT_CMD | Process DShot commands | dshot_commands.txt | dshot_commands.rs:tests | [OK] |
| **Configuration** | | | | |
| REQ-CFG-LAYOUT | Standard EEPROM layout | eeprom_load.txt | - | [OK] |
| REQ-CFG-VERSIONING | Config version migration | - | config.rs:version_defaults_applied_for_old_config | [OK] |
| REQ-CFG-INTEGRITY_CHECK | Detect corrupt flash | eeprom_load.txt | config.rs:blank_flash_is_invalid | [OK] |
| **Features** | | | | |
| REQ-FEAT-BRUSHED_MODE | Brushed motor support | - | brushed.rs:bidirectional_forward | [OK] |
| REQ-MOTOR-SINE_START | Sinusoidal startup | sine_startup.txt | sine.rs:sine_step_changeover_at_high_throttle | [OK] |
| REQ-MOTOR-RAMP_CUSTOM | Acceleration ramps | ramp_rate.txt | - | [OK] |
| REQ-FEAT-PULSE_OUT | Debug pulse output signal | - | - | [BUILD] |

### Summary of Verification
*   **[OK]**: Requirement is verified by a dedicated blackbox vector or Rust unit test.
*   **[BUILD]**: Requirement is verified by the compilation and initialization process (hardware-specific registers, target-specific crates).
*   **[MISSING]**: No dedicated test found.

**Note on Missing Tests:**
- **GCR/NRZI Encoding**: These are lower-level physical layer components. While they are exercised by the higher-level `bidir_dshot.txt` blackbox vector, they lack granular unit tests in the Rust implementation.
