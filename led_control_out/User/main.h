#ifndef _MAIN_H_
#define _MAIN_H_
    





/*  			include start  			*/

/*  			include end  			*/



/*  			define start  			*/

#define RESET_TEMP	35
#define STOP_TEMP	40



#define BUTTON_PORT	GPIOD
#define BUTTON_PIN	GPIO_PIN_3

#define IS_BUTTON_PUSH	(GPIO_ReadInputPin(GPIOD, GPIO_PIN_3) == 0)
#define IS_TOUCH_PUSH	(GPIO_ReadInputPin(GPIOD, GPIO_PIN_4) == 0)


#define IS_COLD_ON	GPIO_ReadInputPin(GPIOA, GPIO_PIN_1)
#define IS_HOT_ON	GPIO_ReadInputPin(GPIOC, GPIO_PIN_4)

#define TIME_5MIN_LED_ON	GPIO_WriteHigh(GPIOD,GPIO_PIN_6)
#define TIME_15MIN_LED_ON	GPIO_WriteHigh(GPIOA,GPIO_PIN_1)
#define TIME_30MIN_LED_ON	GPIO_WriteHigh(GPIOA,GPIO_PIN_2)
#define TIME_60MIN_LED_ON	GPIO_WriteHigh(GPIOA,GPIO_PIN_3)

#define TIME_LED_ALLOFF	GPIO_WriteLow(GPIOD,GPIO_PIN_6);\
						GPIO_WriteLow(GPIOA,GPIO_PIN_1);\
						GPIO_WriteLow(GPIOA,GPIO_PIN_2);\
						GPIO_WriteLow(GPIOA,GPIO_PIN_3);



#define BATTERY_LV1	GPIO_WriteHigh(GPIOC,GPIO_PIN_7)//GPIO_WriteHigh(GPIOA,GPIO_PIN_1);
#define BATTERY_LV2	GPIO_WriteHigh(GPIOC,GPIO_PIN_6)//GPIO_WriteHigh(GPIOA,GPIO_PIN_1);
#define BATTERY_LV3	GPIO_WriteHigh(GPIOC,GPIO_PIN_5)//GPIO_WriteHigh(GPIOA,GPIO_PIN_1);
#define BATTERY_LV4	GPIO_WriteHigh(GPIOC,GPIO_PIN_4)//GPIO_WriteHigh(GPIOA,GPIO_PIN_1);

#define BATTERY_ALLOFF	GPIO_WriteLow(GPIOC,GPIO_PIN_7);\
						GPIO_WriteLow(GPIOC,GPIO_PIN_6);\
						GPIO_WriteLow(GPIOC,GPIO_PIN_5);\
						GPIO_WriteLow(GPIOC,GPIO_PIN_4);

#define COLD_F1_OFF	GPIO_WriteLow(GPIOC,GPIO_PIN_3);//GPIO_WriteLow(GPIOA,GPIO_PIN_1);

#define HOT_F1_ON	TIM1_SetCompare4(63);//GPIO_WriteHigh(GPIOC,GPIO_PIN_4);
#define HOT_F1_OFF	TIM1_SetCompare4(0);//GPIO_WriteLow(GPIOC,GPIO_PIN_4);






/*  			define end  			*/



/*  			enum start  			*/

typedef enum
{
	B_BUTTON,
	B_TOUCH,
} BUTTON_E;
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

typedef enum
{
	BRIGHT_0,
	BRIGHT_1 = 50,
	BRIGHT_2 = 100,
	BRIGHT_3 = 150,
	BRIGHT_4 = 200,
	BRIGHT_5 = 250,
} LED_BRIGHT_E;

typedef enum
{
	STEP_MIN_5 = 1,
	STEP_MIN_15,
	STEP_MIN_30,
	STEP_MIN_60,
} LED_TIME_STEP_E;

typedef enum
{
	STATUS_PUSH,
	STATUS_RELESE,
} LED_BUTTON_E;

typedef enum
{
	MIN_5 = 5000,
	MIN_15 = 15000,
	MIN_30 = 30000,
	MIN_60 = 60000,
} LED_PUSH_E;


/*  			stuct start  			*/
typedef struct
{
	LED_TIME_STEP_E timeStep;
	
	uint8_t ledBrightStep;
	uint32_t offTime;

} LED_S;

typedef struct
{
	STEP_E step; 
	uint32_t push_time;
	uint32_t relese_time;
	uint32_t push_term;
	GPIO_TypeDef* GPIOx;
	GPIO_Pin_TypeDef PortPins;
	int cnt;
	int longcnt;
	void (*shot_push)(); 
	void (*long_push)();

} LED_MEMBER_S;

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
float Check_Battery_ADC(void);
void Check_Usb_ADC(void);

void Battery_Selece();

void Button_config(LED_MEMBER_S* led);

void Button_config_Bright();
void Button_config_Time();

void LED_Bright();
void LED_Bright_Off();
void LED_Time();
void LED_Time_Off();
void ADC_Battery_Enable();
void ADC_Usb_Enable();
void Dumy_func();




/*  			function end  			*/


#endif
