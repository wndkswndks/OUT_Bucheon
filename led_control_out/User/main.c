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
#include<math.h>
/* Private defines -----------------------------------------------------------*/




extern uint32_t TIM1COUNTER;
float adctest = 0;
void main(void)
{

  CLK_DeInit();
  /* Initialization of the clock ;Clock divider to HSI/1 */
  CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);

  GPIO_Config();//4PWM 
  TIM1_Counter_Init();
  TIM4_Config();
  ADC_Config();
  TIM1_SetCompare3(0);

  //halt();
  while (1)
  {
   adctest = Check_Temp();
   Delay(100000);
    //Led_Pwm_config();//
    Button_config();
    //Temp_config();
    //Low_power_Config();

	//WWDG_SetCounter(125);


  }
}



TEMP_MODE_E temp_mode = HOT_MODE;
uint8_t on_off_mode = OFF_MODE;

uint8_t time_15min_flag = RESET;
uint32_t time_15min_cnt = 0;

uint8_t time_1min_flag = RESET;
uint32_t time_1min_cnt = 0;
uint8_t temp_over = 0;


void Led_Pwm_config()////////
{	
	uint8_t is_holding_button = 0;
	static uint8_t holding_start = 0;

	uint32_t holdig_term_ms = 0;
	static uint32_t holding_start_time_ms = 0;
	static uint32_t holding_end_time_ms = 0;
	uint32_t now_time_ms = 0;
	static uint32_t prv_time_ms = 0;
	static uint32_t continuity_push_prevention_ms = 0;
	uint16_t long_define_time_ms = 1000;



	if(!GPIO_ReadInputPin(BUTTON_PORT, BUTTON_PIN)) 
	{
		is_holding_button = SET; 
	}
	else 
	{
		is_holding_button = 0;
	}
//================================	
	if(is_holding_button && !holding_start)
	{
		holding_start_time_ms = HAL_GetTick();
		prv_time_ms = holding_start_time_ms;
		holding_start = SET;
	}
	else if(is_holding_button && holding_start)
	{

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

void Button_config()
{
	static uint8_t step = STEP1; 
	static uint32_t push_time, relese_time = 0;
	uint32_t push_term;

	switch(step)
	{
		case STEP1:
			if(IS_BUTTON_PUSH) 
			{
				if(HAL_GetTick()- relese_time >= 30)
				{
					push_time = HAL_GetTick();
					step = STEP2;
				}
			}
		break;

		case STEP2:
			if(!IS_BUTTON_PUSH ) 
			{
				push_term = HAL_GetTick() -push_time;

				if(30<push_term && push_term<500)
				{
					short_holding_config();
					step = STEP1;
				}
				
				relese_time = HAL_GetTick();
			}
			else if(IS_BUTTON_PUSH && HAL_GetTick() -push_time >1000)
			{
					long_holding_config();
					step = STEP3;
			}
		break;

		case STEP3:
			if(!IS_BUTTON_PUSH ) 
			{
				step = STEP1;
				relese_time = HAL_GetTick();
			}
		break;
	}
	
}


void short_holding_config()
{


}


void long_holding_config()
{	

}

void Low_power_Config()
{
	static uint32_t last_on_time_ms = 0;
	if(IS_HOT_ON ==RESET && IS_COLD_ON ==RESET)
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
void ADC_Config(void)
{
  /*  Init GPIO for ADC2 */
  GPIO_Init(GPIOD, GPIO_PIN_2, GPIO_MODE_IN_FL_NO_IT);
  
  /* De-Init ADC peripheral*/
  ADC1_DeInit();

  /* Init ADC2 peripheral */
  ADC1_Init(ADC1_CONVERSIONMODE_CONTINUOUS, ADC1_CHANNEL_3, ADC1_PRESSEL_FCPU_D2, 
            ADC1_EXTTRIG_TIM, DISABLE, ADC1_ALIGN_RIGHT, ADC1_SCHMITTTRIG_CHANNEL6,
            DISABLE);

  /* Enable EOC interrupt */
  //ADC1_ITConfig(ADC1_IT_AWS6, ENABLE);
  ADC1_ITConfig(ADC1_IT_EOCIE, DISABLE);
  
  ADC1_Cmd(ENABLE);

  /* Enable general interrupts */  
  //enableInterrupts();
  
  /*Start Conversion */
  ADC1_StartConversion();
}


float Vin = 0;
float Check_Temp(void)
{ 
	 static uint16_t sum = 0;
	 uint8_t ADC_NUM = 10;
	 float Avg_Conversion_Value = 0.0;
	 static uint8_t getAdc_cnt = 0;
	 static uint32_t past_time = 0;
	 static uint8_t step = STEP1;
	 float temperature_R = 0;
	 float temp = 0;

	 switch(step)
	 {
		case STEP1:
			if(HAL_GetTick() >= past_time + 10 )
			{
				sum += ADC1_GetConversionValue();
				past_time = HAL_GetTick();
				getAdc_cnt++;
				if(getAdc_cnt ==10) 
				{
					getAdc_cnt = 0;
					step = STEP2;
				}
			}

		break;

		case STEP2:
			Avg_Conversion_Value = (float)sum / ADC_NUM;
			sum = 0;
			Vin = (Avg_Conversion_Value / 1024.0) * 3.3;

			step = STEP1;

		break;
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
