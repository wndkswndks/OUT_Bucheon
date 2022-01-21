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


/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

LED_T m_led;

BUTTON_T m_button;


extern uint32_t TIM1COUNTER;

void main(void)
{

//  CLK_DeInit();
//  /* Initialization of the clock ;Clock divider to HSI/1 */
//  CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);
//  //CLK_HSECmd(ENABLE);
// // CLK_HSICmd(ENABLE);
// //
//  TIM1_Counter_Init();//4PWM 

//  GPIO_DeInit(GPIOD);
//
//  CLK_DeInit();
//	CLK_HSICmd(ENABLE);
//	CLK_SYSCLKConfig(CLK_PRESCALER_HSIDIV1);  //8-8 still normal
//	CLK_SYSCLKConfig(CLK_PRESCALER_CPUDIV1);
//
//	
//  GPIO_DeInit(GPIOD);

  m_button.step = STEP1;
  GPIO_Init(GPIOB,GPIO_PIN_4 , GPIO_MODE_OUT_PP_LOW_FAST);
  GPIO_Init(GPIOB,GPIO_PIN_5 , GPIO_MODE_OUT_PP_LOW_FAST);
  GPIO_Init(GPIOA,GPIO_PIN_3 , GPIO_MODE_OUT_PP_LOW_FAST);
//  TIM4_Config();
  while (1)
  {
  	GPIO_WriteHigh(GPIOB,GPIO_PIN_4);
	Delay(10000);
	GPIO_WriteLow(GPIOB,GPIO_PIN_4);
	Delay(10000);

	GPIO_WriteHigh(GPIOB,GPIO_PIN_5);
	Delay(10000);
	GPIO_WriteLow(GPIOB,GPIO_PIN_5);
	Delay(10000);

	GPIO_WriteHigh(GPIOA,GPIO_PIN_3);
	Delay(10000);
	GPIO_WriteLow(GPIOA,GPIO_PIN_3);
	Delay(10000);

	
//    Led_time_config();//
//    PWR_Config();
//	WWDG_SetCounter(125);
//	Button_step_reset();
//	Led_oprating_config();
  }
}


void Led_time_config()////////
{
	uint32_t holdig_term_ms = 0;
	uint8_t is_holding_button = 0;
	static uint8_t holding_start = 0;
	static uint32_t holding_start_time_ms = 0;
	static uint32_t holding_end_time_ms = 0;
	
	static uint32_t prv_time_ms = 0;
	static uint32_t continuity_push_prevention_ms = 0;
	
	if(!GPIO_ReadInputPin(GPIOD, GPIO_PIN_2)) is_holding_button = 1; 
	else is_holding_button = 0;
//================================	
	if(is_holding_button && !holding_start)
	{
		holding_start_time_ms = HAL_GetTick();
		prv_time_ms = holding_start_time_ms;
		holding_start = SET;
	}
	else if(!is_holding_button && holding_start)
	{
		if(Time_taken(HAL_GetTick(),prv_time_ms) <20) return;		
		holding_end_time_ms = HAL_GetTick();
		holding_start = RESET;
	}
//================================	

	if(holding_start_time_ms && holding_end_time_ms)
	{
		
		holdig_term_ms = Time_taken(holding_end_time_ms,holding_start_time_ms);
		if(Time_taken(HAL_GetTick(),continuity_push_prevention_ms) > 120)
		{
			if(20 < holdig_term_ms && holdig_term_ms < 500 )
			{						
				shot_push_config();
			}
		}
		holding_start_time_ms = 0;
		holding_end_time_ms = 0;

		continuity_push_prevention_ms = HAL_GetTick();
	}
	
}


void shot_push_config()
{
	

	m_led.operating = SET;
	switch(m_button.step)
	{
		case STEP1:		
				m_led.on_status = 1;	
				m_led.eternul = 0;
				m_led.time_cnt = 0;
				m_led.on_time = 3000;
				m_button.step = STEP2;
		break;

		case STEP2:
				m_led.on_status = 2;
				m_led.eternul = 0;
				m_led.on_time = 6000;
				m_led.time_cnt = 0;
				m_button.step = STEP3;
		break;

		case STEP3:
				m_led.on_status = 3;
				m_led.eternul = 1;
				m_led.time_cnt = 0;
				m_button.step = STEP4;
			
		break;

		case STEP4:
				m_led.on_status = 4;
				m_led.eternul = 0;
				m_led.time_cnt = 0;
				m_button.step = STEP1;
		break;

	}
	m_button.push_last_time = HAL_GetTick();

}

uint8_t gpio_A = 0;
uint8_t gpio_B = 0;
void PWR_Config()
{
	static uint32_t last_on_time_ms = 0;
	gpio_B = GPIO_ReadOutputData(GPIOB);
	Delay(1000);
	gpio_A = GPIO_ReadOutputData(GPIOA);
	if(gpio_A == 0 )
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

void Button_step_reset()
{
	if(Time_taken(HAL_GetTick(),m_button.push_last_time)>1000 && m_button.step != STEP1)
	{
		m_button.step = STEP1;
	}
}

void Led_oprating_config()
{
	if(m_led.operating)
	{
		m_led.operating = 0;
		
		switch (m_led.on_status)
		{
			case 1:
			GPIO_WriteLow(LED2_PORT,LED2_PIN);
			Delay(1000);
			GPIO_WriteLow(LED3_PORT,LED3_PIN);
			Delay(1000);
			GPIO_WriteHigh(LED1_PORT,LED1_PIN);
			Delay(1000);
			break;
			
			case 2:
			GPIO_WriteLow(LED1_PORT,LED1_PIN);
			Delay(1000);
			GPIO_WriteLow(LED3_PORT,LED3_PIN);
			Delay(1000);
			GPIO_WriteHigh(LED2_PORT,LED2_PIN);
			Delay(1000);
			break;

			case 3:
			GPIO_WriteLow(LED1_PORT,LED1_PIN);
			Delay(1000);
			GPIO_WriteLow(LED2_PORT,LED2_PIN);
			Delay(1000);
			GPIO_WriteHigh(LED3_PORT,LED3_PIN);
			Delay(1000);
			break;

			case 4:
			GPIO_WriteLow(LED1_PORT,LED1_PIN);
			Delay(1000);
			GPIO_WriteLow(LED2_PORT,LED2_PIN);
			Delay(1000);
			GPIO_WriteLow(LED3_PORT,LED3_PIN);
			Delay(1000);
			break;		
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
