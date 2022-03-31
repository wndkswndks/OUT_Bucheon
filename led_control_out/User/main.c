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
void main(void)//////////ttt///eee////lllllqwer
{

  CLK_DeInit();
  /* Initialization of the clock ;Clock divider to HSI/1 */
  CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);

  GPIO_Config();//4PWM 
  TIM4_Config();
  ADC_Config();

  //halt();
  while (1)
  {
    Led_Pwm_config();//
    Temp_config();
    Low_power_Config();

	//WWDG_SetCounter(125);


  }
}



uint8_t led_status = STEP1;
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

void Cold_ON()
{
	temp_mode = COLD_MODE;	
	time_1min_flag = RESET;
	time_1min_cnt = 0;

	time_15min_cnt = 0;
	time_15min_flag = SET;
	
	HOT_F1_OFF;
	HOT_F2_OFF;
	HOT_LED1_OFF;
	HOT_LED2_OFF;
	
	COLD_F1_ON;
	COLD_F2_ON;
	COLD_LED1_ON;
	COLD_LED2_ON;

}
void Hot_ON()
{
	temp_mode = HOT_MODE;
	time_1min_flag = RESET;
	time_1min_cnt = 0;

	time_15min_cnt = 0;
	time_15min_flag = SET;
	
	COLD_F1_OFF;
	COLD_F2_OFF;
	COLD_LED1_OFF;
	COLD_LED2_OFF;
	
	HOT_F1_ON;
	HOT_F2_ON;
	HOT_LED1_ON;
	HOT_LED2_ON;
}

void All_off(uint8_t fan_on_flag)
{
	
	COLD_F1_OFF;
	COLD_F2_OFF;
	COLD_LED1_OFF;
	COLD_LED2_OFF;
	HOT_F1_OFF;
	HOT_F2_OFF;
	HOT_LED1_OFF;
	HOT_LED2_OFF;
	if(fan_on_flag == RESET) FAN_OFF;
		

	time_15min_cnt = 0;
	time_15min_flag = RESET;
}
void short_holding_config()
{

	if(on_off_mode == OFF_MODE)return;
	
	if(temp_mode == COLD_MODE)
	{
		Hot_ON();
	}
	else if(temp_mode == HOT_MODE)
	{	
		Cold_ON();
	}
	
}


void long_holding_config()
{	
	if(on_off_mode == ON_MODE)
	{
		on_off_mode = OFF_MODE;
		
		All_off(SET);
		
		time_1min_flag = SET;
		time_1min_cnt = 0;
		
	}

	else if(on_off_mode == OFF_MODE)
	{
		on_off_mode = ON_MODE;

		Hot_ON();

		FAN_ON;

		temp_over = RESET;
	}

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


// -21.78 k옴 
//-20 -21.78, 
//-19 -20.59, 
//-18 -19.41, 
//-17 -18.24, 
//-16 -17.08, 
//-15 -15.93, 
//-14 -14.78, 
//-13 -13.64, 
//-12 -12.50, 
//-11 -11.38, 
//-10 -10.26, 
//-9 -9.15, 
//-8 -8.04, 
//-7 -6.94, 
//-6 -5.85, 
//-5 -4.76, 
//-4 -3.68, 
//-3 -2.61, 
//-2 -1.54, 
//-1 -0.48, 
//0 0.60, 
//1 1.63, 
//2 2.68, 
//3 3.72, 
//4 4.75, 
//5 5.78, 
//6 6.81, 
//7 7.83, 
//8 8.84, 
//9 9.85, 
//10 10.85, 
//11 11.85, 
//12 12.85, 
//13 13.84, 
//14 14.83, 
//15 15.81, 
//16 16.78, 
//17 17.75, 
//18 18.72, 
//19 19.68, 
//20 20.64, 
//21 21.59, 
//22 22.54, 
//23 23.49, 
//24 24.42, 
//25 25.38, 
//26 26.29, 
//27 27.21, 
//28 28.13, 
//29 29.05, 
//30 29.96, 
//31 30.87, 
//32 31.77, 
//33 32.67, 
//34 33.57, 
//35 34.45, 
//36 35.34, 
//37 36.22, 
//38 37.09, 
//39 37.96, 
//40 38.83, 
uint32_t GetTick()
{
	return TIM1COUNTER;
}

float temperature = 0;

void Temp_config()
{
	float tmp = 0;
	
	tmp = Check_Temp();

	if(tmp !=0)temperature = tmp;

	if(temperature >=STOP_TEMP)
	{
		temp_over = SET;
		on_off_mode = OFF_MODE;
		
		All_off(RESET);
		
	}

	if(temp_over==SET && temperature <= RESET_TEMP)
	{
		temp_over = RESET;
		on_off_mode = ON_MODE;

		Hot_ON();

		FAN_ON;
		

		time_15min_cnt = 0;
		time_15min_flag = SET;


	}
}

float Check_Temp(void)
{ 
	 static uint16_t sum = 0;
	 uint8_t ADC_NUM = 10;
	 float Avg_Conversion_Value = 0.0;
	 static uint8_t getAdc_cnt = 0;
	 static uint32_t past_time = 0;
	 static uint8_t step = STEP1;
	 float Vin = 0;
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
			Vin = (Avg_Conversion_Value / 1024.0) * 3.0;
			
			temperature_R = Vin/(3.0 - Vin) * 10000.0; //3V, 10K옴 저항분배
			
			temp = 0.0;
			// 온도계 데이터 시트 : MF52B 103F3950-100.zh-CN
			// 엑셀로 저항값들을 나열한뒤 근사치 식을 구했다.
			// 원래식  Y = 32958 * 2.71828^-0.047X
			// 원래식을 x에 관한 식으로 변경
			
			
			temp = log(32958/temperature_R)/0.047;

			step = STEP1;

			return temp;

		break;
	 }
	 
	 

	 return 0;
 
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
