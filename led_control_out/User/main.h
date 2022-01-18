#ifndef _MAIN_H_
#define _MAIN_H_
    





/*  			include start  			*/

/*  			include end  			*/



/*  			define start  			*/
#define LIGHT_DEGREE 15
#define WINDOW_VALUE        97
#define COUNTER_INIT       104

#define LAST_LED_BRIGHT_STEP	16
#define FIRST_LED_BRIGHT_STEP	1

#define BUTTON_PORT	GPIOD
#define BUTTON_PIN	GPIO_PIN_2


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
	CCR1_LED,
	CCR2_LED,
} CCR_E;

/*  			enum end  				*/



/*  			stuct start  			*/

/*  			stuct end  				*/



/*  			function start  		*/
void Led_Pwm_config();
void short_holding_config();
void long_holding_config();
void Add_Led_bright(uint8_t ccr_led, uint16_t add_value);
uint32_t HAL_GetTick();
void TIM4_Config(void);
uint32_t Time_taken(uint32_t nowtime,uint32_t prevtime);
void Low_power_Config();
void Delay(uint32_t cnt);

/*  			function end  			*/


#endif
