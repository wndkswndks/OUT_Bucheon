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
//uint8_t USART1_ReceiveDataBuf[32];

typedef enum
{
	STEP1,
	STEP2,
	STEP3,
	STEP4,
	STEP5,
	STEP6,
	STEP7,
	STEP8,
	STEP9,
	STEP10,
} STEP_E;
typedef enum
{
	CCR1_LED,
	CCR2_LED,
} CCR_E;
#define LIGHT_DEGREE 15
#define WINDOW_VALUE        97
#define COUNTER_INIT       104

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
void button_config2();
void shot_push_config();

uint32_t HAL_GetTick();

void TIM4_Config(void);
uint32_t Time_taken(uint32_t nowtime,uint32_t prevtime);
void PWR_Config();


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
extern uint32_t TIM1COUNTER;

uint8_t led_operate_flag = 0;
uint8_t led_on_off_flag = 0;
uint32_t led_on_time = 0;
uint32_t led_time_cnt = 0;


uint8_t button_status_cnt = 0;


uint8_t button_step = STEP1;
uint32_t push_last_time = 0;
void main(void)//////////ttt///eee////lllllqwer///gg
{

  CLK_DeInit();
  /* Initialization of the clock ;Clock divider to HSI/1 */
  CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);
  //CLK_HSECmd(ENABLE);
 // CLK_HSICmd(ENABLE);
 //
  TIM1_Counter_Init();//4PWM 

  
  TIM4_Config();
  while (1)
  {
    button_config2();//
    
    PWR_Config();

	WWDG_SetCounter(125);


	if(Time_taken(HAL_GetTick(),push_last_time)>1000 && button_step != STEP1)
	{
		button_step = STEP1;
	}

	
	if(led_operate_flag)
	{
		led_operate_flag = 0;
		
		if(led_on_off_flag)
		{
			GPIO_WriteHigh(GPIOC,GPIO_PIN_3);
  			GPIO_WriteHigh(GPIOC,GPIO_PIN_4);
		}
		else
		{
			GPIO_WriteLow(GPIOC,GPIO_PIN_3);
  			GPIO_WriteLow(GPIOC,GPIO_PIN_4);
		}
	}


  }
}


void button_config2()////////
{
	uint32_t term_time = 0;
	uint8_t puch_status = 0;
	static uint8_t puch_flag = 0;
	static uint32_t puch_time = 0;
	static uint32_t relese_time = 0;
	
	static uint32_t prv_time = 0;
	static uint32_t chattering_check_time = 0;
	
	if(!GPIO_ReadInputPin(GPIOD, GPIO_PIN_2)) puch_status = 1; 
	else puch_status = 0;
//================================	
	if(puch_status && !puch_flag)
	{
		puch_time = HAL_GetTick();
		prv_time = puch_time;
		puch_flag = SET;
	}
	else if(!puch_status && puch_flag)
	{
		if(Time_taken(HAL_GetTick(),prv_time) <30) return;		
		relese_time = HAL_GetTick();
		puch_flag = RESET;
	}
//================================	

	if(puch_time && relese_time)
	{
		
		term_time = Time_taken(relese_time,puch_time);
		if(Time_taken(HAL_GetTick(),chattering_check_time) > 150)
		{
			if(30 < term_time && term_time < 500 )
			{						
				shot_push_config();
			}
		}
		puch_time = 0;
		relese_time = 0;

		chattering_check_time = HAL_GetTick();
	}
	
}

uint8_t infinity_flag = 0;
void shot_push_config()
{
	

	
	switch(button_step)
	{
		case STEP1:
				infinity_flag = 0;
				if(led_on_off_flag != SET)
				{
					led_operate_flag = SET;
					led_on_off_flag = SET;
				}
				led_time_cnt = 0;
				led_on_time = 3000;
				button_step = STEP2;
		break;

		case STEP2:
				infinity_flag = 0;
				led_on_time = 6000;
				led_time_cnt = 0;
				button_step = STEP3;
		break;

		case STEP3:
				infinity_flag = 1;
				led_time_cnt = 0;
				button_step = STEP4;
			
		break;

		case STEP4:
				infinity_flag = 0;
				led_operate_flag = SET;
				led_time_cnt = 0;
				led_on_off_flag = RESET;
				button_step = STEP1;
			
		break;

	}
	push_last_time = HAL_GetTick();

}

void PWR_Config()
{
	uint8_t gpio_test = 0;
	static uint32_t last_on_time = 0;

	gpio_test = GPIO_ReadOutputData(GPIOC);
	if(gpio_test == 0 )
    {
		if(Time_taken(HAL_GetTick(),last_on_time)>60000) 
		{
			last_on_time = HAL_GetTick();
			halt();
		}
    }
    else
    {
		last_on_time = HAL_GetTick();
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
