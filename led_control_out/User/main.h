#ifndef _MAIN_H_
#define _MAIN_H_
    





/*  			include start  			*/

/*  			include end  			*/



/*  			define start  			*/

#define RESET_TEMP	35
#define STOP_TEMP	40



#define BUTTON_PORT	GPIOD
#define BUTTON_PIN	GPIO_PIN_3

#define IS_BUTTON_PUSH	(GPIO_ReadInputPin(BUTTON_PORT, BUTTON_PIN) == 0)

#define IS_COLD_ON	GPIO_ReadInputPin(GPIOA, GPIO_PIN_1)
#define IS_HOT_ON	GPIO_ReadInputPin(GPIOC, GPIO_PIN_4)



#define COLD_F1_ON	GPIO_WriteHigh(GPIOC,GPIO_PIN_3);//GPIO_WriteHigh(GPIOA,GPIO_PIN_1);
#define COLD_F1_OFF	GPIO_WriteLow(GPIOC,GPIO_PIN_3);//GPIO_WriteLow(GPIOA,GPIO_PIN_1);

#define HOT_F1_ON	TIM1_SetCompare4(63);//GPIO_WriteHigh(GPIOC,GPIO_PIN_4);
#define HOT_F1_OFF	TIM1_SetCompare4(0);//GPIO_WriteLow(GPIOC,GPIO_PIN_4);






/*  			define end  			*/



/*  			enum start  			*/
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
	HOT_MODE,
	COLD_MODE,
} TEMP_MODE_E;

typedef enum
{
	OFF_MODE,
	ON_MODE,
} ON_OFF_MODE_E;


/*  			enum end  				*/



/*  			stuct start  			*/

/*  			stuct end  				*/



/*  			function start  		*/
void Led_Pwm_config();
void short_holding_config();
void long_holding_config();

uint32_t HAL_GetTick();
void TIM4_Config(void);
uint32_t Time_taken(uint32_t nowtime,uint32_t prevtime);
void Low_power_Config();
void Delay(uint32_t cnt);
void ADC_Config(void);
float Check_Temp(void);

void Button_config();



/*  			function end  			*/


#endif
