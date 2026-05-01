/*
 * comparator.c
 *
 *  Created on: Sep. 26, 2020
 *      Author: Alka
 */

#include "comparator.h"
#include "common.h"
#include "targets.h"

COMP_TypeDef* active_COMP = COMP1;

uint8_t getCompOutputLevel() { /* REQTRK: REQ-HW-COMP-READ */ return LL_COMP_ReadOutputLevel(active_COMP); }

void maskPhaseInterrupts()
{
  /* REQTRK: REQ-HW-COMP-INT_MASK */
  EXTI->IMR &= ~(1 << 21);
  EXTI->PR = EXTI_LINE;
}

void enableCompInterrupts() { /* REQTRK: REQ-HW-COMP-INT_ENABLE */ EXTI->IMR |= (1 << 21); }

void changeCompInput()
{
/* REQTRK: REQ-HW-COMP-CFG */
if((average_interval < 400)){
COMP->CSR = COMP->CSR & ~(1<<2);
}else{
COMP->CSR  = COMP->CSR | 1<<2;
}
  EXTI->RTSR = !rising << 21;
  EXTI->FTSR = rising << 21;
}
