#ifndef _LIGHTRGBWDRV_H_
#define _LIGHTRGBWDRV_H_

#include "stm8s.h"


void GPIO_Config();
void LED_Pwm_ctrl(uint8_t BValue, uint8_t WValue);

#endif