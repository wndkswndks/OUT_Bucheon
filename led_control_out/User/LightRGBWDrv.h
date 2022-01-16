#ifndef _LIGHTRGBWDRV_H_
#define _LIGHTRGBWDRV_H_

#include "stm8s.h"


void TIM1_Counter_Init();
void TIM1RGBWCtrl(uint8_t RValue, uint8_t GValue, uint8_t BValue, uint8_t WValue);

#endif