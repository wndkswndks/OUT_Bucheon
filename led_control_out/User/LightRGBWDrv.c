#include "LightRGBWDrv.h"
#include "stm8s.h"
#include "main.h"


void GPIO_Config()
{
  CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER1, ENABLE);


	GPIO_Init(GPIOA,GPIO_PIN_3 , GPIO_MODE_OUT_PP_LOW_FAST);

	GPIO_Init(GPIOC,GPIO_PIN_3 , GPIO_MODE_OUT_PP_LOW_FAST);
	GPIO_Init(GPIOC,GPIO_PIN_5 , GPIO_MODE_OUT_PP_LOW_FAST);
	GPIO_Init(GPIOC,GPIO_PIN_7 , GPIO_MODE_OUT_PP_LOW_FAST);
	
	GPIO_Init(GPIOD,GPIO_PIN_4 , GPIO_MODE_OUT_PP_LOW_FAST);
	GPIO_Init(GPIOD,GPIO_PIN_6 , GPIO_MODE_OUT_PP_LOW_FAST);

	GPIO_WriteLow(GPIOC,GPIO_PIN_3);
	
	GPIO_WriteLow(GPIOA,GPIO_PIN_3);
	
	GPIO_WriteHigh(GPIOC,GPIO_PIN_5);
	GPIO_WriteHigh(GPIOC,GPIO_PIN_7);
	GPIO_WriteHigh(GPIOD,GPIO_PIN_4);
	GPIO_WriteHigh(GPIOD,GPIO_PIN_6);



  GPIO_Init(GPIOD,GPIO_PIN_3 , GPIO_MODE_IN_FL_IT);
  EXTI_SetExtIntSensitivity(EXTI_PORT_GPIOD, EXTI_SENSITIVITY_FALL_ONLY);
  EXTI_SetTLISensitivity(EXTI_TLISENSITIVITY_FALL_ONLY);

}


void TIM1_Counter_Init()
{
  CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER1, ENABLE);


//  GPIO_Init(GPIOC,GPIO_PIN_3 , GPIO_MODE_OUT_PP_LOW_FAST);
  GPIO_Init(GPIOC,GPIO_PIN_4 , GPIO_MODE_OUT_PP_LOW_FAST);
  
  TIM1_DeInit();
  
  TIM1_TimeBaseInit(64,TIM1_COUNTERMODE_UP, 254,0);//10ms
  
//  TIM1_OC3Init(TIM1_OCMODE_PWM1, TIM1_OUTPUTSTATE_ENABLE, TIM1_OUTPUTNSTATE_DISABLE, 200, TIM1_OCPOLARITY_HIGH, TIM1_OCNPOLARITY_HIGH, TIM1_OCIDLESTATE_SET, TIM1_OCNIDLESTATE_RESET);
//  TIM1_OC3PreloadConfig(ENABLE);
  TIM1_OC4Init(TIM1_OCMODE_PWM1, TIM1_OUTPUTSTATE_ENABLE, 200, TIM1_OCPOLARITY_HIGH, TIM1_OCIDLESTATE_SET);
  TIM1_OC4PreloadConfig(ENABLE);
  TIM1_Cmd(ENABLE);
  TIM1_CtrlPWMOutputs(ENABLE);


//  GPIO_Init(GPIOD,GPIO_PIN_2 , GPIO_MODE_IN_FL_IT);
//  EXTI_SetExtIntSensitivity(EXTI_PORT_GPIOD, EXTI_SENSITIVITY_FALL_ONLY);
//  EXTI_SetTLISensitivity(EXTI_TLISENSITIVITY_FALL_ONLY);

}

void LED_Pwm_ctrl(uint8_t BValue, uint8_t WValue)
{
  TIM1_SetCompare3(BValue);
  TIM1_SetCompare4(WValue);
  
}

