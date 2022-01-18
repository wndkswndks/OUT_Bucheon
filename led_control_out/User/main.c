/**
  ******************************************************************************
  * @file    Project/main.c 
  * @author  MCD Application Team
  * @version V2.1.0
  * @date    18-November-2011
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */ 


/* Includes ------------------------------------------------------------------*/
#include "stm8s.h"
#include "main.h"
#include "LightRGBWDrv.h"
/* Private defines -----------------------------------------------------------*/




extern uint32_t TIM1COUNTER;
void main(void)//////////ttt///eee////lllllqwer
{

  CLK_DeInit();
  /* Initialization of the clock ;Clock divider to HSI/1 */
  CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);

  TIM1_Counter_Init();//4PWM 
  TIM4_Config();
  LED_Pwm_ctrl(0,0);//50% PWM  test
  while (1)
  {
    Led_Pwm_config();//
    Low_power_Config();

	//WWDG_SetCounter(125);


  }
}



uint8_t led_status = STEP1;


void Led_Pwm_config()////////
{
	uint16_t led_bright_step1 = 0;
	uint16_t led_bright_step2 = 0;
	
	uint8_t is_holding_button = 0;
	static uint8_t holding_start = 0;

	uint32_t holdig_term_ms = 0;
	static uint32_t holding_start_time_ms = 0;
	static uint32_t holding_end_time_ms = 0;
	uint32_t now_time_ms = 0;
	static uint32_t prv_time_ms = 0;
	static uint32_t continuity_push_prevention_ms = 0;
	uint16_t long_define_time_ms = 500;


	led_bright_step1 = TIM1_GetCapture3()/LIGHT_DEGREE;
	led_bright_step2 = TIM1_GetCapture4()/LIGHT_DEGREE;

	if(!GPIO_ReadInputPin(BUTTON_PORT, BUTTON_PIN)) is_holding_button = SET; 
	else is_holding_button = 0;
//================================	
	if(is_holding_button && !holding_start)
	{
		holding_start_time_ms = HAL_GetTick();
		prv_time_ms = holding_start_time_ms;
		holding_start = SET;
	}
	else if(is_holding_button && holding_start)
	{
		if(led_bright_step1 ==LAST_LED_BRIGHT_STEP ||led_bright_step2 ==LAST_LED_BRIGHT_STEP ) 
			long_define_time_ms = 3000;
		else 
			long_define_time_ms = 500;

		now_time_ms = HAL_GetTick();
		if(Time_taken(now_time_ms,prv_time_ms) > long_define_time_ms)
		{
			long_holding_config();
			prv_time_ms = now_time_ms;
		}
	}	
	else if(!is_holding_button && holding_start)
	{
		if(Time_taken(HAL_GetTick(),prv_time_ms) <30) return;	// up chettering prevention
		if(Time_taken(HAL_GetTick(),now_time_ms) <30) return;	// up chettering prevention
		
		holding_end_time_ms = HAL_GetTick();
		holding_start = RESET;
	}
//================================	

	if(holding_start_time_ms && holding_end_time_ms)
	{	
		holdig_term_ms = Time_taken(holding_end_time_ms,holding_start_time_ms);
		if(Time_taken(HAL_GetTick(),continuity_push_prevention_ms) > 200)
		{
			if(30 < holdig_term_ms && holdig_term_ms < 500 )
			{
				short_holding_config();
			}
		}
		holding_start_time_ms = 0;
		holding_end_time_ms = 0;
		continuity_push_prevention_ms = HAL_GetTick();
	}
	

}

void short_holding_config()
{
	static uint8_t oneflag = 0;
	static uint8_t step = STEP1;
	static uint16_t memory_ccr1 = 0;

	switch(step)
	{
		case STEP1:
			if(led_status == STEP1)
			{
				if(oneflag==0) TIM1_SetCompare3(LIGHT_DEGREE*8);
				else TIM1_SetCompare3(memory_ccr1);
				
				step = STEP2;
			}
		break;

		case STEP2:
			if(led_status == STEP1)
			{
				TIM1_SetCompare3(0);
				TIM1_SetCompare4(LIGHT_DEGREE*8);
				led_status = STEP2;
				step = STEP3;
			}
		break;

		case STEP3:
			if(led_status == STEP2)
			{
				TIM1_SetCompare3(LIGHT_DEGREE*10);
				TIM1_SetCompare4(LIGHT_DEGREE*10);
				led_status = STEP3;
				step = STEP4;
			}
		break;

		case STEP4:
			if(led_status == STEP3)
			{
				memory_ccr1 = TIM1_GetCapture3();
				TIM1_SetCompare3(0);
				TIM1_SetCompare4(0);
				led_status = STEP1;
				oneflag = 1;
				step = STEP1;
			}
		break;

	}


}

void long_holding_config()
{
	switch(led_status)
	{
		case STEP1:
			Add_Led_bright(CCR1_LED, LIGHT_DEGREE);			
		break;

		case STEP2:
			Add_Led_bright(CCR2_LED, LIGHT_DEGREE);
		break;
		
		case STEP3:
			Add_Led_bright(CCR1_LED, LIGHT_DEGREE);
			Add_Led_bright(CCR2_LED, LIGHT_DEGREE);
		break;
	}
}
void Add_Led_bright(uint8_t ccr_led, uint16_t add_value)
{
	uint16_t data = 0;
	
	switch(ccr_led)
	{
		case CCR1_LED:
		data = TIM1_GetCapture3();
		data += add_value;
		if(data>LIGHT_DEGREE*LAST_LED_BRIGHT_STEP) 
			TIM1_SetCompare3(LIGHT_DEGREE*FIRST_LED_BRIGHT_STEP); 
		else 
			TIM1_SetCompare3(data);
		break;

		case CCR2_LED:
		data = TIM1_GetCapture4();
		data += add_value;
		if(data>LIGHT_DEGREE*LAST_LED_BRIGHT_STEP) 
			TIM1_SetCompare4(LIGHT_DEGREE*FIRST_LED_BRIGHT_STEP);
		else 
			TIM1_SetCompare4(data);
		break;
	}
}

void Low_power_Config()
{
	static uint32_t last_on_time_ms = 0;
	if(TIM1_GetCapture3() == 0 && TIM1_GetCapture4() == 0)
    {
		if(Time_taken(HAL_GetTick(),last_on_time_ms)>60000) 
		{
			last_on_time_ms = HAL_GetTick();
			halt();
		}
    }
    else
    {
		last_on_time_ms = HAL_GetTick();
    }
}


#define TIM4_PERIOD       124

void TIM4_Config(void)
{
  /* TIM4 configuration:
   - TIM4CLK is set to 16 MHz, the TIM4 Prescaler is equal to 128 so the TIM1 counter
   clock used is 16 MHz / 128 = 125 000 Hz
  - With 125 000 Hz we can generate time base:
      max time base is 2.048 ms if TIM4_PERIOD = 255 --> (255 + 1) / 125000 = 2.048 ms
      min time base is 0.016 ms if TIM4_PERIOD = 1   --> (  1 + 1) / 125000 = 0.016 ms
  - In this example we need to generate a time base equal to 1 ms
   so TIM4_PERIOD = (0.001 * 125000 - 1) = 124 */

  /* Time base configuration */
  TIM4_TimeBaseInit(TIM4_PRESCALER_128, TIM4_PERIOD);
  /* Clear TIM4 update flag */
  TIM4_ClearFlag(TIM4_FLAG_UPDATE);
  /* Enable update interrupt */
  TIM4_ITConfig(TIM4_IT_UPDATE, ENABLE);
  
  /* enable interrupts */
  enableInterrupts();

  /* Enable TIM4 */
  TIM4_Cmd(ENABLE);
}


uint32_t Time_taken(uint32_t nowtime,uint32_t prevtime)
{
	if(nowtime >=prevtime) return (nowtime - prevtime);
	else return ((nowtime+0xFFFF) - prevtime);
	
}

void Delay(uint32_t cnt)
{
  uint32_t count = 500;
  for(; cnt > 0;cnt --)
  {
    for(; count > 0;count --)
    {
    }
  }
}

#ifdef USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param file: pointer to the source file name
  * @param line: assert_param error line source number
  * @retval : None
  */
void assert_failed(u8* file, u32 line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
