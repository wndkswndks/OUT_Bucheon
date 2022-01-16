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
void long_push_config();

void Add_CRR(uint8_t ccr_led, uint16_t add_value);
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

void main(void)//////////ttt///eee////
{

  CLK_DeInit();
  /* Initialization of the clock ;Clock divider to HSI/1 */
  CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);
  //CLK_HSECmd(ENABLE);
 // CLK_HSICmd(ENABLE);
 //
  TIM1_Counter_Init();//4PWM 
  TIM4_Config();
 TIM1RGBWCtrl(0,0,0,0);//50% PWM  test
  while (1)
  {
    button_config2();//
    PWR_Config();

	//WWDG_SetCounter(125);


  }
}


uint16_t led_step1 = 0;
uint16_t led_step2 = 0;

uint8_t led_status = STEP1;

void button_config2()////////
{
	uint32_t term_time = 0;
	uint8_t puch_status = 0;
	static uint8_t puch_flag = 0;
	static uint32_t puch_time = 0;
	static uint32_t relese_time = 0;
	
	uint32_t now_time = 0;
	static uint32_t prv_time = 0;
	static uint32_t chattering_check_time = 0;
	uint16_t long_define_time = 500;
	
	if(!GPIO_ReadInputPin(GPIOD, GPIO_PIN_2)) puch_status = 1; 
	else puch_status = 0;
//================================	
	if(puch_status && !puch_flag)
	{
		puch_time = HAL_GetTick();
		prv_time = puch_time;
		puch_flag = SET;
	}
	else if(puch_status && puch_flag)
	{
		if(led_step1 ==16 ||led_step2 ==16 ) long_define_time = 3000;
		else long_define_time = 500;

		now_time = HAL_GetTick();
		if(Time_taken(now_time,prv_time) > long_define_time)
		{
			long_push_config();
			prv_time = now_time;
		}
	}	
	else if(!puch_status && puch_flag)
	{
		if(Time_taken(HAL_GetTick(),prv_time) <30) return;	
		if(Time_taken(HAL_GetTick(),now_time) <30) return;		
		relese_time = HAL_GetTick();
		puch_flag = RESET;
	}
//================================	

	if(puch_time && relese_time)
	{
		
		term_time = Time_taken(relese_time,puch_time);
		if(Time_taken(HAL_GetTick(),chattering_check_time) > 200)
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
	
	led_step1 = TIM1_GetCapture3()/LIGHT_DEGREE;
	led_step2 = TIM1_GetCapture4()/LIGHT_DEGREE;
}

void shot_push_config()
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

void long_push_config()
{
	switch(led_status)
	{
		case STEP1:
			Add_CRR(CCR1_LED, LIGHT_DEGREE);			
		break;

		case STEP2:
			Add_CRR(CCR2_LED, LIGHT_DEGREE);
		break;
		
		case STEP3:
			Add_CRR(CCR1_LED, LIGHT_DEGREE);
			Add_CRR(CCR2_LED, LIGHT_DEGREE);
		break;
	}
}
void Add_CRR(uint8_t ccr_led, uint16_t add_value)
{
	uint16_t data = 0;
	
	switch(ccr_led)
	{
		case CCR1_LED:
		data = TIM1_GetCapture3();
		data += add_value;
		if(data>LIGHT_DEGREE*16) TIM1_SetCompare3(add_value); 
		else TIM1_SetCompare3(data);
		break;

		case CCR2_LED:
		data = TIM1_GetCapture4();
		data += add_value;
		if(data>LIGHT_DEGREE*16) TIM1_SetCompare4(add_value);
		else TIM1_SetCompare4(data);
		break;
	}
}

void PWR_Config()
{
	static uint32_t last_on_time = 0;
	if(TIM1_GetCapture3() == 0 && TIM1_GetCapture4() == 0)
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
