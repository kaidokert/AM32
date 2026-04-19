#pragma once
#include <stdint.h>

void changeToOutput(void);
void changeToInput(void);
void receiveDshotDma(void);
void sendDshotDma(void);
uint8_t getInputPinState(void);
void setInputPolarityRising(void);
void setInputPullDown(void);
void setInputPullUp(void);
void enableHalfTransferInt(void);
void setInputPullNone(void);

extern volatile char send_telemetry;
