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
LED_S m_led;
LED_MEMBER_S m_led_bright;
LED_MEMBER_S m_led_time;


uint8_t batteryCheck = 0;


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

  m_led.timeStep = STEP_MIN_5;
  
  m_led_bright.GPIOx = GPIOD;
  m_led_bright.PortPins = GPIO_PIN_4;
  m_led_bright.fp = LED_Bright;
  
  m_led_time.GPIOx = GPIOD;
  m_led_time.PortPins = GPIO_PIN_3;  
  m_led_time.fp = LED_Time;

  while (1)
  {
    
    Button_config(&m_led_bright);
    Button_config(&m_led_time);
    LED_Time_Off();
    Check_Battery_ADC();
	Check_Usb_ADC();
    
    Low_power_Config();

	//WWDG_SetCounter(125);


  }
}






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

void Button_config(LED_MEMBER_S* led)
{
//	static uint8_t step = STEP1; 
//	static uint32_t push_time = 0;
//	static uint32_t relese_time = 0;
//	uint32_t push_term = 0;
	
	uint8_t buttonStatus = GPIO_ReadInputPin(led->GPIOx, led->PortPins) ;	
	
	switch(led->step)
	{
		case STEP1:
			if(buttonStatus == STATUS_PUSH) 
			{
				if(HAL_GetTick()- led->relese_time >= 30)
				{
					led->push_time = HAL_GetTick();
					led->step = STEP2;
				}
			}
		break;

		case STEP2:
			led->push_term = HAL_GetTick() -led->push_time;

			
			if(buttonStatus != STATUS_PUSH &&(30<led->push_term && led->push_term<500) ) 
			{
				//short_holding_config();
				led->fp();
				led->cnt++;
				led->relese_time = HAL_GetTick();
				led->step = STEP1;
				
			}
			else if(buttonStatus == STATUS_PUSH && led->push_term >1000)
			{
				//long_holding_config();
				led->longcnt++;
				led->step = STEP3;
			}
		break;

		case STEP3:
			if(buttonStatus != STATUS_PUSH ) 
			{
				led->step = STEP1;
				led->relese_time = HAL_GetTick();
			}
		break;
	}
	
}


void Button_config_Bright()
{
	static uint8_t step = STEP1; 
	static uint32_t push_time = 0;
	static uint32_t relese_time = 0;
	uint32_t push_term = 0;
	uint8_t buttonStatus = 0;	
	
	buttonStatus = IS_TOUCH_PUSH; 
	switch(step)
	{
		case STEP1:
			if(buttonStatus) 
			{
				if(HAL_GetTick()- relese_time >= 30)
				{
					push_time = HAL_GetTick();
					step = STEP2;
				}
			}
		break;

		case STEP2:
			push_term = HAL_GetTick() -push_time;

			
			if(!buttonStatus&&(30<push_term && push_term<500) ) 
			{
				short_holding_config();

				LED_Bright();
				
				relese_time = HAL_GetTick();
				step = STEP1;
				
			}
			else if(buttonStatus && push_term >1000)
			{
				long_holding_config();
				
				LED_Bright_Off();

				step = STEP3;
			}
		break;

		case STEP3:
			if(!buttonStatus ) 
			{
				step = STEP1;
				relese_time = HAL_GetTick();
			}
		break;
	}
	
}

void Button_config_Time()
{
	static uint8_t step = STEP1; 
	static uint32_t push_time = 0;
	static uint32_t relese_time = 0;
	uint32_t push_term = 0;
	uint8_t buttonStatus = 0;
	
	
	buttonStatus = IS_BUTTON_PUSH;
	switch(step)
	{
		case STEP1:
			if(buttonStatus) 
			{
				if(HAL_GetTick()- relese_time >= 30)
				{
					push_time = HAL_GetTick();
					step = STEP2;
				}
			}
		break;

		case STEP2:
			push_term = HAL_GetTick() -push_time;

			
			if(!buttonStatus&&(30<push_term && push_term<500) ) 
			{
				short_holding_config();

				LED_Time();

				
				relese_time = HAL_GetTick();
				step = STEP1;
				
			}
			else if(buttonStatus && push_term >1000)
			{
				long_holding_config();

				step = STEP3;
			}
		break;

		case STEP3:
			if(!buttonStatus ) 
			{
				step = STEP1;
				relese_time = HAL_GetTick();
			}
		break;
	}
	
}

void LED_Bright()
{
	if(HAL_GetTick()>m_led.offTime)
	{
		switch(m_led.timeStep)
		{
			case STEP_MIN_5:
				TIME_5MIN_LED_ON;
				m_led.offTime = HAL_GetTick() + (uint32_t)MIN_5;
			break;

			case STEP_MIN_15:
				TIME_15MIN_LED_ON;
				m_led.offTime = HAL_GetTick() + (uint32_t)MIN_15;
			break;

			case STEP_MIN_30:
				TIME_30MIN_LED_ON;
				m_led.offTime = HAL_GetTick() + (uint32_t)MIN_30;
			break;

			case STEP_MIN_60:
				TIME_60MIN_LED_ON;
				m_led.offTime = HAL_GetTick() + (uint32_t)MIN_60;
			break;
			
		}
	}
	
	switch(m_led.ledBrightStep)
	{
		case BRIGHT_0 :
			batteryCheck = 1;
			for(int i =0 ;i < 5;i++)
			{
				m_led.ledBrightStep += 10;
				TIM1_SetCompare3(m_led.ledBrightStep);
				Delay(100);
			}
		break;	

		case BRIGHT_1 :
		case BRIGHT_2 :
		case BRIGHT_3 :
		case BRIGHT_4 :
			for(int i =0 ;i < 5;i++)
			{
				m_led.ledBrightStep += 10;
				TIM1_SetCompare3(m_led.ledBrightStep);
				Delay(100);
			}
		break;

		case BRIGHT_5 :
			m_led.ledBrightStep = BRIGHT_1;
			TIM1_SetCompare3(m_led.ledBrightStep);
		break;
	}

}
void LED_Bright_Off()
{
	TIME_LED_ALLOFF;
	m_led.offTime = 0;
	m_led.ledBrightStep = BRIGHT_0;
	TIM1_SetCompare3(0);
}

void LED_Time()
{
	m_led.timeStep++;
	if(m_led.timeStep>STEP_MIN_60)m_led.timeStep=STEP_MIN_5;

	TIME_LED_ALLOFF;
	switch(m_led.timeStep)
	{
		case STEP_MIN_5:
			TIME_5MIN_LED_ON;
			m_led.offTime = HAL_GetTick() + (uint32_t)MIN_5;
		break;

		case STEP_MIN_15:
			TIME_15MIN_LED_ON;
			m_led.offTime = HAL_GetTick() + (uint32_t)MIN_15;
		break;

		case STEP_MIN_30:
			TIME_30MIN_LED_ON;
			m_led.offTime = HAL_GetTick() + (uint32_t)MIN_30;
		break;

		case STEP_MIN_60:
			TIME_60MIN_LED_ON;
			m_led.offTime = HAL_GetTick() + (uint32_t)MIN_60;
		break;
	}
}

void LED_Time_Off()
{
	if(HAL_GetTick() > m_led.offTime)
	{
		TIM1_SetCompare3(0);
		TIME_LED_ALLOFF;
		m_led.ledBrightStep = BRIGHT_0;
	}
}

int buttonCnt = 0;
int buttonLongCnt = 0;

void short_holding_config()
{
	buttonCnt++;
}


void long_holding_config()
{	
	buttonLongCnt++;
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



void Delay(uint32_t Delay)
{
  uint32_t destTimd = HAL_GetTick()+Delay;

  while (destTimd >HAL_GetTick() )
  {
  }
}



void ADC_Config(void)
{

  GPIO_Init(GPIOD, GPIO_PIN_2, GPIO_MODE_IN_FL_NO_IT);

  GPIO_Init(GPIOD, GPIO_PIN_5, GPIO_MODE_IN_FL_NO_IT);
  ADC1_DeInit();


  ADC1_Init(ADC1_CONVERSIONMODE_CONTINUOUS, ADC1_CHANNEL_5, ADC1_PRESSEL_FCPU_D2, 
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


void ADC_Battery_Enable()
{
	ADC1_Init(ADC1_CONVERSIONMODE_CONTINUOUS, ADC1_CHANNEL_3, ADC1_PRESSEL_FCPU_D2, 
			  ADC1_EXTTRIG_TIM, DISABLE, ADC1_ALIGN_RIGHT, ADC1_SCHMITTTRIG_CHANNEL6,
			  DISABLE);
}

void ADC_Usb_Enable()
{

	ADC1_Init(ADC1_CONVERSIONMODE_CONTINUOUS, ADC1_CHANNEL_5, ADC1_PRESSEL_FCPU_D2, 
			  ADC1_EXTTRIG_TIM, DISABLE, ADC1_ALIGN_RIGHT, ADC1_SCHMITTTRIG_CHANNEL6,
			  DISABLE);
}

float VinUsb = 0;
float VinBattry = 0;

void Check_Usb_ADC(void)
{
	float ADC = 0;
	 static uint32_t past_time = 0;
	  static uint32_t on_time = 0;
	 if(batteryCheck==1) return;

	 if(HAL_GetTick()- past_time >= 500 )
	 {	
		
			ADC = ADC1_GetConversionValue();
			VinUsb = (ADC / 1024.0) * 3.3;


			if(VinUsb>2)
			{
				ADC_Battery_Enable();
				ADC = ADC1_GetConversionValue();
				ADC_Usb_Enable();
				VinBattry = (ADC / 1024.0) * 3.3 *3.6875;

				if(4.2<=VinBattry)
				{
					BATTERY_LV4;
					BATTERY_LV3;
					BATTERY_LV2;
					BATTERY_LV1;
				}
				else if((3.9<=VinBattry)&&(VinBattry<4.2)) 
				{
					BATTERY_LV3;
					BATTERY_LV2;
					BATTERY_LV1;
				}
				else if((3.6<=VinBattry)&&(VinBattry<3.9)) 
				{
					BATTERY_LV2;
					BATTERY_LV1;
				}
				else if(VinBattry<3.6)
				{
					BATTERY_LV1;
				}
				on_time = HAL_GetTick();
				
			}
		
		past_time = HAL_GetTick();
	}

	if(HAL_GetTick()- on_time >= 250 )
	{
		BATTERY_ALLOFF;
	}
}

float Check_Battery_ADC(void)
{ 
	 float ADC = 0;
	 static uint16_t sum = 0;
	 uint8_t ADC_NUM = 10;
	 static float Avg_Conversion_Value = 0.0;
	 static uint8_t getAdc_cnt = 0;
	 static uint32_t past_time = 0;
	 static uint8_t step = STEP1;
	 float temperature_R = 0;
	 float temp = 0;
	 
	 switch(step)
	 {
	 	case STEP1:
			if(batteryCheck)
			{	
				ADC_Battery_Enable();
				step = STEP2;
			}
	 	break;	 	

		case STEP2:
			ADC = ADC1_GetConversionValue();
			VinBattry = (ADC / 1024.0) * 3.3 *3.6875;

			if(4.2<=VinBattry)
			{
				BATTERY_LV4;
				BATTERY_LV3;
				BATTERY_LV2;
				BATTERY_LV1;
			}
			else if((3.9<=VinBattry)&&(VinBattry<4.2)) 
			{
				BATTERY_LV3;
				BATTERY_LV2;
				BATTERY_LV1;
			}
			else if((3.6<=VinBattry)&&(VinBattry<3.9)) 
			{
				BATTERY_LV2;
				BATTERY_LV1;
			}
			else if(VinBattry<3.6)
			{
				BATTERY_LV1;
			}

			past_time = HAL_GetTick();
			step = STEP3;

		break;

		case STEP3:
			if(HAL_GetTick()- past_time >= 3000 )
			{
				ADC_Usb_Enable();
				batteryCheck = 0;
				BATTERY_ALLOFF;
				step = STEP1;
			}
		break;

	 }
	 
 
}

uint8_t toggle = 0;
void Battery_Selece()
{
	 static uint32_t past_time = 0;

	 if(HAL_GetTick()- past_time >= 8000 )
	 {
	 	batteryCheck = 1;


		if(toggle ==1)
		{
			ADC_Battery_Enable();
			toggle = 0;
		}
		else
		{
			ADC_Usb_Enable();
			toggle = 1;
		}
		//Delay(100);
		past_time = HAL_GetTick();
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
