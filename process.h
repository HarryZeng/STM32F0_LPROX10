///**
//  ********************************  STM32F0xx  *********************************
//  * @文件名     ： process.c
//  * @作者       ： HarryZeng
//  * @库版本     ： V1.5.0
//  * @文件版本   ： V1.0.0
//  * @日期       ： 2017年04月21日
//  * @摘要       ： 数据处理
//  ******************************************************************************/
///*----------------------------------------------------------------------------
//  更新日志:
//  2017-04-21 V1.0.0:初始版本
//  ----------------------------------------------------------------------------*/
///* 包含的头文件 --------------------------------------------------------------*/
/* Includes ------------------------------------------------------------------*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __process_H
#define __process_H
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include  "stm32f0xx.h"
#include  "stm32f0xx_adc.h"
#define ADC_IN_Pin 					GPIO_Pin_2
#define ADC_IN_GPIO_Port 		GPIOA
#define PWM1_Pin 						GPIO_Pin_0
#define PWM1_GPIO_Port 			GPIOA

#define SW1_LD_Pin 							GPIO_Pin_3
#define SW1_LD_GPIO_Port 				GPIOB
#define SW1_ONOFF_Pin 					GPIO_Pin_4
#define SW1_ONOFF_GPIO_Port 		GPIOB

#define ALM_Pin 						GPIO_Pin_11
#define ALM_GPIO_Port 			GPIOA
#define OUT_Pin 						GPIO_Pin_10
#define OUT_GPIO_Port 			GPIOA	

#define SC_Pin 							GPIO_Pin_9
#define SC_GPIO_Port 				GPIOA		 
	 
#define Algorithm_P				4096	 
	 
#define 	FLASH_START_ADDR 	 						0x08007000

/*应差值*/	

#define PWM1_HIGH  	1000
#define PWM1_LOW  	100
	
#define StartTime  	240



/*PWM通道状态控制 宏定义*/
/*W8SET的PWMX,PWMY,PWMZ,PWM1的输出*/
#define PWM1_ON  	GPIO_WriteBit(PWM1_GPIO_Port, PWM1_Pin, Bit_SET)
#define PWM1_OFF  GPIO_WriteBit(PWM1_GPIO_Port, PWM1_Pin, Bit_RESET)

#define PWMX_ON  	GPIO_WriteBit(PWMX_GPIO_Port, PWMX_Pin, Bit_SET)
#define PWMX_OFF  GPIO_WriteBit(PWMX_GPIO_Port, PWMX_Pin, Bit_RESET)

//#define PWMY_ON  	GPIO_WriteBit(PWMY_GPIO_Port, PWMY_Pin, Bit_SET)
//#define PWMY_OFF  GPIO_WriteBit(PWMY_GPIO_Port, PWMY_Pin, Bit_RESET)

#define PWMZ_ON  	GPIO_WriteBit(PWMZ_GPIO_Port, PWMZ_Pin, Bit_SET)
#define PWMZ_OFF  GPIO_WriteBit(PWMZ_GPIO_Port, PWMZ_Pin, Bit_RESET)

/*W8--Dual的PWM输出*/
#define PWMY_ON  	GPIOA->ODR = GPIOA->ODR | 0x0080  
#define PWMY_OFF  GPIOA->ODR = GPIOA->ODR & 0xFF7F 



#define MainTIMER  TIM3


#define _Gpio_7_tro  GPIO_WriteBit(GPIOA, GPIO_Pin_7, (BitAction)!GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_7))
#define _Gpio_7_set  GPIO_WriteBit(GPIOA, GPIO_Pin_7, Bit_SET)
#define _Gpio_7_reset  GPIO_WriteBit(GPIOA, GPIO_Pin_7, Bit_RESET)
	 
typedef enum
{
  PWMX = 0U,
  PWMY,
	PWMZ
}PWM_Number;
	 

typedef enum
{
  PWM_ON = 0U,
	PWM_OFF
}PWM_STATE;
	 


void DataProcess(void);
void ResetParameter(void);


#ifdef __cplusplus
}
#endif
#endif 
/**** Copyright (C)2017 HarryZeng. All Rights Reserved **** END OF FILE ****/

