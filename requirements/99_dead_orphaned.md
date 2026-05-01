# 99 Dead & Orphaned Logic

This document catalogs logic present in the original C firmware that is either functionally dead (set but never read), orphaned (defined but never called), or explicitly excluded from future implementations. These are tracked here to ensure codebase traceability without cluttering active functional requirements.

## Hardware & Silicon Quirks

- **DEAD-HW-F051_TEMP_OFFSET:** Early silicon temperature offsets for F051.
  - **Status:** Dead Code. The `temperature_offset` variable is set based on `REV_Id` but is never utilized in any temperature calculation or telemetry path.
  - **C Reference:** `Src/main.c:1836-1838`

- **DEAD-HW-BRIDGE_ENABLE:** Specific GPIO toggling for driver enable pins (e.g., PA12, PF6/PF7) on legacy MCUs.
  - **Status:** Out of Scope. These refer to MCUs (F031, G031) not targeted by the current 4-chip set (F051, G071, L431, G431).
  - **C Reference:** `Src/main.c:1756-1763`

## Protocol & Signal Processing

- **DEAD-SIG-MULTISHOT:** Support for the legacy MultiShot protocol.
  - **Status:** Orphaned/Excluded. The `computeMSInput` function exists in the C source but is not called by the central signal dispatcher. Explicitly declined for the Rust rewrite.
  - **C Reference:** `Src/signal.c:102` (`computeMSInput`)

- **DEAD-FEAT-HALL_SENSORS:** Sensored BLDC operation via internal/external Hall sensors.
  - **Status:** Unimplemented Placeholder. The variable `use_hall_sensors` is defined and forced to 0 in C, and is explicitly marked as "Not implemented in C or Rust" in the Rust config.
  - **C Reference:** `Src/main.c:678` (`#ifndef HAS_HALL_SENSORS`)
  - **Rust Reference:** `downloads/rm32/src/config.rs` (`reserved_hall_sensors`)

## Excluded Features

- **REQ-FEAT-DroneCAN:** CAN-bus based control and telemetry.
  - **Status:** Out of Scope. Intentionally excluded from the current phase of the Rust rewrite.
  - **C Reference:** `Src/DroneCAN/`
