#include <stdint.h>

long map(long x, long in_min, long in_max, long out_min, long out_max);
uint32_t getAbsDif(int number1, int number2);
void delayMillis(uint32_t millis);
void delayMicros(uint32_t micros);

typedef enum
{
  RESET = 0U,
  SET = !RESET
} FlagStatus, ITStatus;
