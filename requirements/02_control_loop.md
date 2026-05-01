# 02 Control Loop & Protection

This document details the main control loop logic, state transitions, and safety protection mechanisms.

## State Machine

- **REQ-STATE-ARMED:** The motor MUST only spin when an "Armed" state is active, triggered by a zero-throttle signal for a specified duration.
  - **C Reference:** `Src/main.c:1330`
  - **Rust Reference:** `downloads/rm32/src/control/tick.rs:442-452`

- **REQ-STATE-RUNNING:** The "Running" state MUST be entered when throttle exceeds the minimum threshold and exited on stop or protection event.
  - **C Reference:** `Src/main.c:1474`
  - **Rust Reference:** `downloads/rm32/src/control/isr_logic.rs:50`

## Safety Protections

- **REQ-PROT-LVC:** Monitor battery voltage and reduce power/shut down if below threshold.
  - **C Reference:** `Src/main.c:2061-2070`
  - **Rust Reference:** `downloads/rm32/src/control/tick.rs:636-654`

- **REQ-PROT-STUCK_ROTOR:** Shut down if zero-cross not detected within BEMF timeout window.
  - **C Reference:** `Src/main.c:1106-1114`
  - **Rust Reference:** `downloads/rm32/src/control/tick.rs:135-143`

- **REQ-PROT-STALL_TIMEOUT_ADAPT:** Dynamic adjustment of stall detection timeout based on throttle and state.
  - **C Reference:** `Src/main.c:1040-1061`
  - **Rust Reference:** `downloads/rm32/src/control/tick.rs`

- **REQ-PROT-DESYNC_DETECTION:** Detection of sudden commutation interval deviations indicating desync.
  - **C Reference:** `Src/main.c:1063-1080`
  - **Rust Reference:** `downloads/rm32_stm32/src/main_loop.rs:72-92`

- **REQ-PROT-ERPM_LIMIT:** Restrict maximum duty cycle at low eRPM levels.
  - **C Reference:** `Src/main.c:2116-2117`
  - **Rust Reference:** `downloads/rm32/src/control/tick.rs:658-663`

- **REQ-PROT-TEMP_LIMIT:** Linearly reduce power if temperature exceeds safety threshold.
  - **C Reference:** `Src/main.c:2124-2127`
  - **Rust Reference:** `downloads/rm32/src/control/tick.rs:657-664`

- **REQ-PROT-RC_CAR_REVERSE:** Specialized bidirectional control for RC cars (double-tap to reverse, proportional braking).
  - **C Reference:** `Src/main.c:950-1050`
  - **Rust Reference:** `downloads/rm32/src/control/tick.rs:337-380`
