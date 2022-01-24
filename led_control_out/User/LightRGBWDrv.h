#ifndef _LIGHTRGBWDRV_H_
#define _LIGHTRGBWDRV_H_

#include "stm8s.h"


void GPIO_Config_Init();
void TIM1RGBWCtrl(uint8_t RValue, uint8_t GValue, uint8_t BValue, uint8_t WValue);

#endif