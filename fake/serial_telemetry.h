#pragma once
#include <stdint.h>

void telem_UART_Init(void);
void send_telem_DMA(uint8_t bytes);
