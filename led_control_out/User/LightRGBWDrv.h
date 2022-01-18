#ifndef _LIGHTRGBWDRV_H_
#define _LIGHTRGBWDRV_H_

#include "stm8s.h"


void TIM1_Counter_Init();
void LED_Pwm_ctrl(uint8_t BValue, uint8_t WValue);

#endif