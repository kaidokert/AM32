# AM32 Requirements Index

This directory contains the reverse-engineered requirements for the AM32 firmware, mapped to both the original C implementation and the `rm32` Rust rewrite.

## Requirement Categories

### 1. [Non-Functional Requirements](./01_non_functional.md)
Hardware targets (STM32F0/G0/L4), flash footprints, and timing performance constraints.

### 2. [Control Loop & Protection](./02_control_loop.md)
Main loop state machine, timing intervals, and safety protections (LVC, desync, stuck rotor).

### 3. [Signal Processing & Input](./03_signal_processing.md)
Input detection, DShot decoding (including Bidirectional/CRC), Servo PWM measurement, and frame timing calibration.

### 4. [Motor Control & PID](./04_motor_control.md)
Commutation logic, BEMF polling/interrupts, advance timing, PID loops (Current, Speed, Stall), and Sine/Gimbal modes.

### 5. [Telemetry & Commands](./05_telemetry_commands.md)
Extended DShot Telemetry (EDT) scheduling, DShot command processing (1-47), and interactive programming mode.

### 6. [Configuration & EEPROM](./06_configuration.md)
EEPROM layout, version migrations, default values, and dynamic scaling/calibration.

### 7. [Feedback & UI](./07_feedback.md)
Motor-driven audible tones (Startup, Beacons, Errors, BlueJay parser) and WS2812 LED status indications.

### 8. [Specialized Motor Modes](./08_special_motor_modes.md)
Alternative operation modes such as Brushed Mode, Gimbal Mode, and Bench Testing modes (Fixed Duty/Speed).

### 9. [Advanced Features & Timing](./09_advanced_features.md)
Nuanced features like Custom Ramps, Startup Boost, Hall Sensor support, and PWM Enable Bridge variations.

### 10. [Edge Cases & Test-Derived Logic](./10_test_derived.md)
Fine-grained requirements extracted from blackbox vectors and unit tests, including precise timing thresholds and PID anti-windup behavior.

---

## Codebase References

- **C Source (AM32):** `Src/`, `Inc/`, `Mcu/`
- **Rust Source (rm32):** `downloads/rm32/src/`, `downloads/rm32_stm32/src/`
