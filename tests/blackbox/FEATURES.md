# AM32 Rust Rewrite — Feature Roadmap & Acceptance Criteria

Prioritized backlog for reimplementing AM32 firmware in Rust.
Each feature has blackbox test vectors that must produce identical
output between the C reference and the Rust implementation.

## Priority Levels

- **P0 — Must have for first flight**: Core motor control loop.
  Without these, the ESC does nothing.
- **P1 — Must have for safe operation**: Protection features.
  Without these, hardware can be damaged.
- **P2 — Must have for real-world use**: Input protocols, telemetry.
  Without these, it can't talk to a flight controller.
- **P3 — Nice to have**: Advanced features, edge cases.
  Can be deferred without blocking initial testing.

## Status Key

- [x] Blackbox vector exists and passes
- [~] Partial coverage
- [ ] No vector yet

---

## P0 — Core Motor Control

These form the inner control loop. Get these right first.

| # | Feature | Vector | BB Coverage | Notes |
|---|---------|--------|-------------|-------|
| 1 | **6-step commutation (forward)** | [x] `motor_commutation` | commutate 100% | Step 1→6 wrap, rising/falling |
| 2 | **6-step commutation (reverse)** | [~] `bidir_dshot` | commutate 100% | Step 6→1 wrap |
| 3 | **BEMF zero-cross detection** | [x] `motor_commutation` | interruptRoutine 100% | Filter level, comp polarity |
| 4 | **Commutation timing/advance** | [x] `motor_commutation` | PeriodElapsedCallback 77% | waitTime, advance calc |
| 5 | **Polling→interrupt changeover** | [~] `motor_commutation` | zcfoundroutine 81% | old_routine→0 transition |
| 6 | **Duty cycle ramp limiting** | [x] `ramp_rate` | tenKhzRoutine 78% | Startup/low/high RPM rates |
| 7 | **PWM output** | [x] `dshot_input` | tenKhzRoutine 78% | adjusted_duty_cycle, SET_DUTY_CYCLE_ALL |
| 8 | **Motor startup sequence** | [~] `motor_commutation` | startMotor 0% BB | Via setInput, needs explicit vector |
| 9 | **eRPM calculation** | [~] `motor_commutation` | main_loop 50% | e_com_time → e_rpm |
| 10 | **Desync detection/recovery** | [ ] | main_loop 50% | Needs vector with interval jumps |

### Remaining P0 work:
- [ ] Vector for explicit `startMotor` path (running=0 → running=1)
- [ ] Vector for desync detection (rapid commutation_interval change)
- [ ] Vector for auto_advance calculation

---

## P1 — Protection Features

Safety-critical. Must work before flying.

| # | Feature | Vector | BB Coverage | Notes |
|---|---------|--------|-------------|-------|
| 1 | **BEMF timeout (stuck rotor)** | [x] `bemf_timeout` | setInput 62% | Cuts throttle on stall |
| 2 | **Low voltage cutoff (per-cell)** | [x] `low_voltage_cutoff` | main_loop 50% | Cell count × threshold |
| 3 | **Low voltage cutoff (absolute)** | [ ] | main_loop 50% | `low_voltage_cut_off=2` path |
| 4 | **Temperature limiting** | [x] `temperature_limit` | main_loop 50% | duty_cycle_maximum reduction |
| 5 | **Current limiting PID** | [x] `pid_loops` | doPidCalculations 100% | Anti-windup covered |
| 6 | **Stall protection PID** | [x] `pid_loops` | tenKhzRoutine 78% | Throttle boost at low RPM |
| 7 | **Signal timeout disarm** | [x] `signal_timeout` | main_loop 50% | Half-second armed, 2s unarmed |
| 8 | **Watchdog** | n/a | n/a | RELOAD_WATCHDOG_COUNTER is no-op in harness |

### Remaining P1 work:
- [ ] Vector for absolute voltage cutoff (`low_voltage_cut_off=2`)
- [ ] Vector for filter_level adjustment at different speeds

---

## P2 — Input & Telemetry

Required to communicate with flight controller.

| # | Feature | Vector | BB Coverage | Notes |
|---|---------|--------|-------------|-------|
| 1 | **DShot decode** | [x] `dshot_input` | computeDshotDMA 74% | Frame decode, CRC, throttle |
| 2 | **DShot bidirectional** | [x] `dshot_telemetry` | transfercomplete 88% | Inverted CRC, telem response |
| 3 | **DShot GCR telemetry encode** | [~] `dshot_telemetry` | make_dshot_package 58% | eRPM encoding, shift amount |
| 4 | **DShot commands** | [x] `dshot_commands` + `_extended` | computeDshotDMA 74% | Cmds 1-14, 20-21, 36 |
| 5 | **DShot programming mode** | [x] `dshot_commands_extended` | computeDshotDMA 74% | Position+value+commit |
| 6 | **Servo PWM detection** | [x] `servo_input` | detectInput 84%, checkServo 100% | Auto-detect |
| 7 | **Servo PWM decode** | [x] `servo_input` | computeServoInput 59% | Pulse → throttle mapping |
| 8 | **Servo calibration** | [x] `servo_calibration` | computeServoInput 59% | High+low end averaging |
| 9 | **KISS telemetry packets** | [x] `telemetry` | makeTelemPackage 100% | Temp/voltage/current/eRPM/CRC |
| 10 | **ESC info packet** | [x] `telemetry` | makeInfoPacket 100% | EEPROM dump + CRC |
| 11 | **Telemetry interval timer** | [x] `telemetry` | tenKhzRoutine 78% | Periodic send |
| 12 | **EDT extended telemetry** | [~] `dshot_commands_extended` | make_dshot_package 58% | Temp/voltage/current scheduling |
| 13 | **Input auto-detection** | [~] `servo_input` | detectInput 84% | DShot vs servo |
| 14 | **Arming sequence** | [x] `arming_basic` | tenKhzRoutine 78% | Zero-throttle timeout |

### Remaining P2 work:
- [ ] More `make_dshot_package` branches (EDT scheduling, shift amounts)
- [ ] DShot 300 vs 600 detection (checkDshot branches, currently 15%)
- [ ] MultiShot input (computeMSInput, currently 0%)

---

## P3 — Advanced Features

Can be deferred for initial bring-up.

| # | Feature | Vector | BB Coverage | Notes |
|---|---------|--------|-------------|-------|
| 1 | **Bidirectional throttle (DShot)** | [x] `bidir_dshot` | setInput 62% | >1047 fwd, 48-1047 rev |
| 2 | **Bidirectional throttle (servo)** | [ ] | setInput 62% | Servo neutral + dead band |
| 3 | **RC car reverse mode** | [x] `rc_car_reverse` | setInput 62% | Prop brake + return to center |
| 4 | **Sine startup mode** | [x] `sine_startup` | setInput 62% | Stepper→BEMF changeover |
| 5 | **Variable PWM mode 1** | [x] `variable_pwm` | main_loop 50% | Frequency-based ARR |
| 6 | **Variable PWM mode 2** | [ ] | main_loop 50% | Auto range ARR |
| 7 | **Brake on stop** | [~] `brake_modes` | setInput 62% | Non-comp_pwm path |
| 8 | **Drag brake** | [~] `brake_modes` | setInput 62% | comp_pwm + brake_on_stop=1 |
| 9 | **Active brake** | [ ] | tenKhzRoutine 78% | brake_on_stop=2, comStep(2) |
| 10 | **Proportional brake** | [~] `rc_car_reverse` | setInput 62% | RC car prop brake duty |
| 11 | **Speed control loop** | [~] `pid_loops` | tenKhzRoutine 78% | drive_by_rpm, input_override |
| 12 | **EEPROM settings load** | [~] `pid_loops` | loadEEpromSettings 54% | Version migration, defaults |
| 13 | **EEPROM settings save** | [x] `dshot_commands_extended` | saveEEpromSettings 100% | Via DShot cmd 12 |
| 14 | **Sinusoidal drive** | [ ] | advanceincrement 0% BB | Phase angle stepping |
| 15 | **EDT arming mode** | [ ] | setInput 62% | EDTARM_IN input type |
| 16 | **DroneCAN** | n/a | n/a | Not compiled in fake build |

### Remaining P3 work:
- [ ] Bidirectional servo vector
- [ ] Variable PWM mode 2 vector
- [ ] Active brake vector (brake_on_stop=2)
- [ ] Sinusoidal drive / advanceincrement vector
- [ ] EDT arming flow vector
- [ ] Direction reversal at speed (blocked vs allowed)

---

## Summary

| Priority | Features | Vectors | Status |
|----------|----------|---------|--------|
| **P0** Core control | 10 | 7 existing | ~80% covered |
| **P1** Protection | 8 | 6 existing | ~85% covered |
| **P2** Input/Telem | 14 | 12 existing | ~75% covered |
| **P3** Advanced | 16 | 8 existing | ~50% covered |
| **Total** | **48** | **20 vector files** | |

## Recommended order for Rust rewrite

1. **Implement P0 first**: commutate → BEMF → ramp → PWM output.
   Run `motor_commutation` and `ramp_rate` vectors against Rust.
2. **Add P1 protections**: LVC, temp limit, current limit, signal timeout.
   These are simple checks in the control loop.
3. **Wire up P2 input**: DShot decode first (most common), then servo.
   Run `dshot_input`, `arming_basic`, `telemetry` vectors.
4. **P3 as needed**: Bidirectional, RC car, sine start — add when
   specific hardware testing demands it.

## Golden trace strategy

For maximum confidence, record full tick-by-tick output from the C
harness for each vector file and save as `.golden` files. The Rust
implementation must produce byte-identical output. Any divergence
is a bug in the rewrite.

```bash
# Record golden trace
echo "config ..." | ./build/am32_harness > vectors/arming_basic.golden

# Verify Rust matches
echo "config ..." | ./target/release/am32_harness_rs > /tmp/test.out
diff vectors/arming_basic.golden /tmp/test.out
```
