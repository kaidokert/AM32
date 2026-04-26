#pragma once

// Resets ALL firmware globals to their compile-time init values.
// Call this at the start of every test to ensure deterministic state.
#ifdef __cplusplus
extern "C" {
#endif
void resetFirmwareState(void);
#ifdef __cplusplus
}
#endif
