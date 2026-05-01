# AM32 Requirements Index

This directory contains reverse-engineered requirements with semantic tracking slugs.

## Requirement Categories

### 1. [Non-Functional & HW](./01_non_functional.md)
Slugs: `REQ-HW-xxx`, `REQ-RES-xxx`, `REQ-SAFE-xxx`

### 2. [Control Loop & Protection](./02_control_loop.md)
Slugs: `REQ-STATE-xxx`, `REQ-PROT-xxx`

### 3. [Signal Processing & Input](./03_signal_processing.md)
Slugs: `REQ-SIG-xxx`

### 4. [Motor Control & PID](./04_motor_control.md)
Slugs: `REQ-MOTOR-xxx`

### 5. [Telemetry & Commands](./05_telemetry_commands.md)
Slugs: `REQ-TELEM-xxx`

### 6. [Configuration & EEPROM](./06_configuration.md)
Slugs: `REQ-CFG-xxx`

### 7. [Feedback & UI](./07_feedback.md)
Slugs: `REQ-UI-xxx`

### 8. [Specialized Motor Modes](./08_special_motor_modes.md)
Slugs: `REQ-FEAT-xxx`

### 9. [Advanced Features & Variations](./09_advanced_features.md)
Advanced timing, hardware support, and protocol variations.

### 10. [Test-Derived Logic](./10_test_derived.md)
Slugs: `REQ-TEST-xxx` (Precise thresholds and edge cases).

### 11. [Bootloader Interface](./11_bootloader_interface.md)
Slugs: `REQ-HW-BOOTLOADER_xxx`, `REQ-CFG-BOOTLOADER_xxx`

### 99. [Dead & Orphaned Logic](./99_dead_orphaned.md)
Slugs: `DEAD-xxx`

---

## Verification & Tracking
- **[Test Verification Matrix](./TEST_VERIFICATION.md)**: Cross-reference of requirements to blackbox and unit tests.
- **Codebase Tracking**: Use `/* REQTRK: SLUG */` comments in the C codebase to map logic back to these requirements.
