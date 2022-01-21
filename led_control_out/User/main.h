#ifndef _MAIN_H_
#define _MAIN_H_

/*  			include start  			*/

/*  			include end  			*/



/*  			define start  			*/
#define LED1_PORT  GPIOA	
#define LED1_PIN   GPIO_PIN_3	

#define LED2_PORT  GPIOB	
#define LED2_PIN   GPIO_PIN_5

#define LED3_PORT  GPIOB	
#define LED3_PIN   GPIO_PIN_4



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

/*  			enum end  				*/



/*  			stuct start  			*/
typedef struct
{
	uint8_t on_status;
	uint32_t on_time;
	uint8_t operating;
	uint32_t time_cnt;
	uint8_t eternul;
} LED_T;

typedef struct
{
	STEP_E step;
	uint32_t push_last_time;
} BUTTON_T;
/*  			stuct end  				*/



/*  			function start  		*/
void Delay(uint32_t cnt);
void Led_time_config();
void shot_push_config();
uint32_t HAL_GetTick();
void TIM4_Config(void);
uint32_t Time_taken(uint32_t nowtime,uint32_t prevtime);
void PWR_Config();
void Button_step_reset();
void Led_oprating_config();


/*  			function end  			*/





#endif
