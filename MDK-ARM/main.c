#include  "stm32f0xx.h"
#include "stdio.h"
#include "process.h"
#include "flash.h"

#define DMA_BUFFER_SIZE     1  


//  if ((GPIOx->ODR & GPIO_Pin) != (uint32_t)Bit_RESET)
//  {
//    bitstatus = (uint8_t)Bit_SET;
//  }
//  else
//  {
//    bitstatus = (uint8_t)Bit_RESET;
//  }


//if (BitVal != Bit_RESET)
//  {
//    GPIOx->BSRR = GPIO_Pin;
//  }
//  else
//  {
//    GPIOx->BRR = GPIO_Pin ;
//  }

uint8_t sample_finish = 0;  
int16_t adc_dma_tab[6] = { 0 };  
uint8_t sample_index = 0;  
uint8_t PWM_Step=0;

uint8_t PWM_Start_Flag = 0;
uint8_t PWM_Start_Counter = 0;
uint8_t PWM_TimeCounter = 0;

//?????  
int16_t sample_1[128] = { 0 };  
int16_t sample_2[128] = { 0 };  
int16_t sample_3[128] = { 0 };  
int16_t sample_4[128] = { 0 };  
int16_t sample_5[128] = { 0 };  
int16_t sample_6[128] = { 0 };  
  

RCC_ClocksTypeDef   SysClock;
extern void scan_key(void);
extern uint16_t scan_tick;
extern uint8_t EnterSelfFlag;
extern uint8_t 	ShortCircuitCounter;
extern uint8_t 	ShortCircuit;
extern uint8_t FB_Flag;
extern uint8_t PVD_Flag;

void 	adc_config(void) ;
void 	user_adc_init(void)  ;
void 	adc_gpio_init(void);
void 	adc_dma_init(void) ;
void 	USART_Config(void);
void 	GPIO_PWMOUT_Config(void);
void 	TIM1_PWM_Config(void);
void 	TIM1_Base_Config(void);
void	TIM2_PWM_Config(void);
void	TIM3_Config(void);
void TIM14_Config(void);

void GPIO_DEINIT_ALL(void);
/*****************************************/
///////////////////////////////////////////////////////////////////////////////////
/**
  * @brief  //�������´���,֧��printf����,������Ҫѡ��use MicroLIB
  */
int fputc(int ch, FILE *f)
{
	while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
		;
	USART_SendData(USART1, (unsigned char) ch);
  return (ch);
}

void BSP_init(void)  
{  
    adc_gpio_init();  
    adc_config();               // ??????????,????????????????????  
    adc_dma_init();             //  
		//TIM1_PWM_Config();
		//TIM1_Base_Config();
		//TIM2_PWM_Config();
		//TIM14_Config();
		TIM3_Config();
		GPIO_PWMOUT_Config();
		GPIO_DEINIT_ALL();
    //USART_Config();
}

void USART_Config(void)
{
        USART_InitTypeDef USART_InitStructure;
        /*???USART1??*/
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
        USART_InitStructure.USART_BaudRate = 115200;//???????
        USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//?????
        USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;//??????
        USART_InitStructure.USART_Parity = USART_Parity_No;//?????
        USART_InitStructure.USART_StopBits = USART_StopBits_1;//?????
        USART_InitStructure.USART_WordLength = USART_WordLength_8b;//?????
        USART_Init(USART1, &USART_InitStructure);
        USART_Cmd(USART1, ENABLE);//???? 1
}
/*GPIOA????? */
void GPIO_PWMOUT_Config(void)
{
        GPIO_InitTypeDef GPIO_InitStructure;
        /*???GPIOA??*/
        RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
        /* ??PA9 ,PA10*/
        GPIO_InitStructure.GPIO_Pin = PWM1_Pin;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT; //NORMAL
        GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
				GPIO_Init(PWM1_GPIO_Port, &GPIO_InitStructure);
}
/*?? 1 ????*/
void UART_send_byte(uint8_t byte)
{
        while(!((USART1->ISR)&(1<<7)));//?????
        USART1->TDR=byte; //??????
}
/*?????*/
void UART_Send(uint8_t *Buffer, uint32_t Length)
{
        while(Length != 0)
        {
                while(!((USART1->ISR)&(1<<7)));//?????
                USART1->TDR= *Buffer;
                Buffer++;
                Length--;
        }
}

uint8_t UART_Recive(void)
{
        while(!(USART1->ISR & (1<<5)));
        return(USART1->RDR);
}
  
void adc_config()  
{  
    ADC_InitTypeDef adc_init_structure;  

		RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);            //??ADC?? 
    
		ADC_DeInit(ADC1);                                               //??ADC  
    ADC_StructInit(&adc_init_structure);                            //???ADC???  
  
    adc_init_structure.ADC_ContinuousConvMode = DISABLE;            //????????  
    adc_init_structure.ADC_DataAlign = ADC_DataAlign_Right;         //???????  
    //adc_init_structure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_TRGO; //???????TIM1
    //adc_init_structure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_RisingFalling;//?????  
    adc_init_structure.ADC_Resolution = ADC_Resolution_12b;         //12????  
    adc_init_structure.ADC_ScanDirection = ADC_ScanDirection_Upward;//????0-18??  
		
    ADC_Init(ADC1, &adc_init_structure);  
		
    ADC_OverrunModeCmd(ADC1, ENABLE);                               //????????  
    ADC_ChannelConfig(ADC1, ADC_Channel_2, ADC_SampleTime_1_5Cycles);               //??????,????125nS  
    ADC_GetCalibrationFactor(ADC1);                                 //?????ADC  
    ADC_Cmd(ADC1, ENABLE);                                          //??ADC1  
		while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_ADRDY)){} 
    while(ADC_GetFlagStatus(ADC1, ADC_FLAG_ADEN) == RESET);         //??ADC1????  
  
    //ADC_DMACmd(ADC1, ENABLE);                                       //??ADC_DMA  
    //ADC_DMARequestModeConfig(ADC1, ADC_DMAMode_Circular);           //??DMA?????????  
 
}  
  
void adc_gpio_init()  
{
    GPIO_InitTypeDef gpio_init_structure;  
    //??GPIO??  
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA , ENABLE);  
  
    GPIO_StructInit(&gpio_init_structure);  
    //GPIOA                                                         //PA-0~3??ADC  
    gpio_init_structure.GPIO_Pin = (GPIO_Pin_2);  
    gpio_init_structure.GPIO_Mode = GPIO_Mode_AN;                   //????(??)??  
    gpio_init_structure.GPIO_OType = GPIO_OType_PP;                 //????  
    gpio_init_structure.GPIO_Speed = GPIO_Speed_2MHz;              //Fast speed  
    gpio_init_structure.GPIO_PuPd= GPIO_PuPd_UP;                    //??  
    GPIO_Init(GPIOA, &gpio_init_structure);  
}  
  
void adc_dma_init()  
{  
    DMA_InitTypeDef dma_init_structure;  
    NVIC_InitTypeDef nvic_init_structure;  
  
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);              //??DMA??  
  
    nvic_init_structure.NVIC_IRQChannel = DMA1_Channel1_IRQn;       //??DMA1????  
    nvic_init_structure.NVIC_IRQChannelCmd = ENABLE;                //????  
    nvic_init_structure.NVIC_IRQChannelPriority = 0;                //?????0  
    NVIC_Init(&nvic_init_structure);  
  
    DMA_DeInit(DMA1_Channel1);                                      //��λDMA1_channel1  
    DMA_StructInit(&dma_init_structure);                            //��ʼ��DMA�ṹ�� 
  
    dma_init_structure.DMA_BufferSize = DMA_BUFFER_SIZE;            //DMA���������С  
    dma_init_structure.DMA_DIR = DMA_DIR_PeripheralSRC;             //DMA����:������Ϊ����Դ 
    dma_init_structure.DMA_M2M = DISABLE;                           //�ڴ浽�ڴ����
    dma_init_structure.DMA_MemoryBaseAddr = (uint32_t)&adc_dma_tab[0];//��������������ʼ��ַ
    dma_init_structure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;//���ݴ�С����ΪHalfword  
    dma_init_structure.DMA_MemoryInc = DMA_MemoryInc_Enable;        //�ڴ��ַ���� 
    dma_init_structure.DMA_Mode = DMA_Mode_Circular;                //DMAѭ��ģʽ,��ɺ����¿�ʼ���� 
    dma_init_structure.DMA_PeripheralBaseAddr = (uint32_t) &(ADC1->DR);//ȡֵ�������ַ 
    dma_init_structure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;//����ȡֵ��С����ΪHalfword  
    dma_init_structure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//�����ַ��������
    dma_init_structure.DMA_Priority = DMA_Priority_High;             //DMA���ȼ�����Ϊ��  
    DMA_Init(DMA1_Channel1, &dma_init_structure);  
  
    DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, ENABLE);                  //ʹ��DMA�ж�
    DMA_ClearITPendingBit(DMA_IT_TC);                                //���һ��DMA�жϱ�־  
    DMA_Cmd(DMA1_Channel1, ENABLE);                                  //ʹ��DMA1  
}  
  
void TIM1_Base_Config(void)
{
	  TIM_TimeBaseInitTypeDef timer_init_structure;  
    NVIC_InitTypeDef nvic_init_structure;  

		RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1 , ENABLE);	

    nvic_init_structure.NVIC_IRQChannel = TIM1_BRK_UP_TRG_COM_IRQn;                //ʹ��TIM1�ж�ͨ��  
    nvic_init_structure.NVIC_IRQChannelCmd = ENABLE;                //ʹ��TIM1�ж�  
    nvic_init_structure.NVIC_IRQChannelPriority = 8;                //���ȼ�Ϊ0  
		
    NVIC_Init(&nvic_init_structure); 

		/* Time ??????*/
		timer_init_structure.TIM_Prescaler = 5;  //48/6=8 ,8M->0.125us
		timer_init_structure.TIM_CounterMode = TIM_CounterMode_Up;  /* Time ????????????*/
		timer_init_structure.TIM_Period = 640;   // 960->120us  
		timer_init_structure.TIM_RepetitionCounter = 0;

		TIM_TimeBaseInit(TIM1, &timer_init_structure);
		/* TIM1 ?????*/
		//TIM_SelectOutputTrigger(TIM1, TIM_TRGOSource_Update);							//ѡ��TIM1��timerΪ����Դ  
		//TIM_SelectOutputTrigger(TIM1, TIM_TRGOSource_OC1Ref);							//ѡ��TIM1��timerΪ����Դ  
		//TIM_SelectOutputTrigger(TIM1, TIM_TRGOSource_OC2Ref);							//ѡ��TIM1��timerΪ����Դ  
		//TIM_SelectOutputTrigger(TIM1, TIM_TRGOSource_OC3Ref);							//ѡ��TIM1��timerΪ����Դ  
		//TIM_SelectSlaveMode(TIM1, TIM_SlaveMode_Gated);//����ģʽֻ�������ſ�����ͣ�����Կ���
		TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);                      //ʹ��TIM1�ж�
		TIM_ClearITPendingBit(TIM1, TIM_IT_Update);     //���update�¼��жϱ�־

		//TIM_Cmd(TIM1, ENABLE);

		/* TIM1 ????? */
}


void TIM1_PWM_Config(void)
{
	  TIM_TimeBaseInitTypeDef timer_init_structure;  
		TIM_OCInitTypeDef timer_OCinit_structure; 
    NVIC_InitTypeDef nvic_init_structure;  
  	GPIO_InitTypeDef gpio_init_structure;
	
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1 , ENABLE);	
//		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA , ENABLE);	
//	
//    gpio_init_structure.GPIO_Pin = PWMX_Pin;  
//    gpio_init_structure.GPIO_Mode = GPIO_Mode_AF;                   //????(??)??  
//    gpio_init_structure.GPIO_Speed = GPIO_Speed_50MHz;              //Fast speed  
//    gpio_init_structure.GPIO_PuPd= GPIO_PuPd_NOPULL;                    //??  
//    GPIO_Init(PWMX_GPIO_Port, &gpio_init_structure);
//		gpio_init_structure.GPIO_Pin = PWMY_Pin;  
//		GPIO_Init(PWMY_GPIO_Port, &gpio_init_structure);
//		gpio_init_structure.GPIO_Pin = PWMZ_Pin;  
//		GPIO_Init(PWMZ_GPIO_Port, &gpio_init_structure);
//	
//		GPIO_PinAFConfig(GPIOA,GPIO_PinSource8,GPIO_AF_2);
//		GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_2);
//		GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_2);
//	
//    nvic_init_structure.NVIC_IRQChannel = TIM1_BRK_UP_TRG_COM_IRQn;                //ʹ��TIM1�ж�ͨ��  
//    nvic_init_structure.NVIC_IRQChannelCmd = ENABLE;                //ʹ��TIM1�ж�  
//    nvic_init_structure.NVIC_IRQChannelPriority = 2;                //���ȼ�Ϊ0  
//		
//    NVIC_Init(&nvic_init_structure); 


//		/* TIM1 ??? ---------------------------------------------------
//		 TIM1 ????(TIM1CLK) ??? APB2 ?? (PCLK2)    
//			=> TIM1CLK = PCLK2 = SystemCoreClock
//		 TIM1CLK = SystemCoreClock, Prescaler = 0, TIM1 counter clock = SystemCoreClock
//		 SystemCoreClock ?48 MHz 
//		 
//		 ??????? 4 ?PWM ???17.57 KHz:
//			 - TIM1_Period = (SystemCoreClock / 17570) - 1
//		 ??1??????? 50%
//		 ??2??????? 37.5%
//		 ??3??????? 25%
//		 ??4??????? 12.5%
//		 ????????????:
//			 - ChannelxPulse = DutyCycle * (TIM1_Period - 1) / 100
//		*/  
//			/*???????,???????????????*/
//		//TimerPeriod = (SystemCoreClock / 17570 ) - 1;
//		//TimerPeriod = (SystemCoreClock / DEF_PWMFRE ) - 1;
//		//TimerPeriod = (SystemCoreClock / DEF_PWMFRE);
//		/* TIM1 ???? */


//		/* Time ??????*/
//		timer_init_structure.TIM_Prescaler = 1;  //24/6=8 ,8M->0.125us
//		timer_init_structure.TIM_CounterMode = TIM_CounterMode_Up;  /* Time ????????????*/
//		timer_init_structure.TIM_Period = 720;   // 720->60us  
//		timer_init_structure.TIM_RepetitionCounter = 0;

//		TIM_TimeBaseInit(TIM1, &timer_init_structure);

//		/* ??1,2,3,4?PWM ???? */
//		timer_OCinit_structure.TIM_OCMode = TIM_OCMode_PWM1;
//		timer_OCinit_structure.TIM_OutputState = TIM_OutputState_Enable ;//TIM_OutputState_Enable; //PWM?????
//		timer_OCinit_structure.TIM_OutputNState = TIM_OutputNState_Disable ;//TIM_OutputNState_Enable; //??PWM?????
//		timer_OCinit_structure.TIM_OCPolarity = TIM_OCPolarity_High;  //PWM 1?????
//		timer_OCinit_structure.TIM_OCNPolarity = TIM_OCNPolarity_Low; //PWM?? 0?????
//		timer_OCinit_structure.TIM_OCIdleState = TIM_OCIdleState_Set;
//		timer_OCinit_structure.TIM_OCNIdleState = TIM_OCIdleState_Reset;

//		timer_OCinit_structure.TIM_Pulse = PWMx_HIGH; //?????
//		TIM_OC1Init(TIM1, &timer_OCinit_structure);//????1??
//		TIM_OC1PreloadConfig(TIM1,TIM_OCPreload_Disable);

//		timer_OCinit_structure.TIM_Pulse = PWMy_HIGH; //?????
//		TIM_OC2Init(TIM1, &timer_OCinit_structure);//????1??
//		TIM_OC2PreloadConfig(TIM1,TIM_OCPreload_Disable);

//		timer_OCinit_structure.TIM_Pulse = PWMz_HIGH; //?????
//		TIM_OC3Init(TIM1, &timer_OCinit_structure);//????1??
//		TIM_OC3PreloadConfig(TIM1,TIM_OCPreload_Disable);

//		TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);                      //ʹ��TIM1�ж�
////		TIM_ITConfig(TIM1, TIM_IT_CC1, ENABLE);                      //ʹ��TIM1�ж�
//		TIM_ITConfig(TIM1, TIM_IT_CC2, ENABLE);                      //ʹ��TIM1�ж�
////		TIM_ITConfig(TIM1, TIM_IT_CC3, ENABLE);                      //ʹ��TIM1�ж�

//		//TIM_ARRPreloadConfig(TIM1,DISABLE);

//		/* TIM1 ?????*/
//		//TIM_SelectOutputTrigger(TIM1, TIM_TRGOSource_Update);							//ѡ��TIM1��timerΪ����Դ  
//		//TIM_SelectOutputTrigger(TIM1, TIM_TRGOSource_OC1Ref);							//ѡ��TIM1��timerΪ����Դ  
//		//TIM_SelectOutputTrigger(TIM1, TIM_TRGOSource_OC2Ref);							//ѡ��TIM1��timerΪ����Դ  
//		//TIM_SelectOutputTrigger(TIM1, TIM_TRGOSource_OC3Ref);							//ѡ��TIM1��timerΪ����Դ  
//		//TIM_SelectSlaveMode(TIM1, TIM_SlaveMode_Gated);//����ģʽֻ�������ſ�����ͣ�����Կ���
//		TIM_ClearITPendingBit(TIM1, TIM_IT_Update);     //���update�¼��жϱ�־
//		TIM_ClearITPendingBit(TIM1, TIM_IT_CC1);     //���update�¼��жϱ�־
//		TIM_ClearITPendingBit(TIM1, TIM_IT_CC2);     //���update�¼��жϱ�־
//		TIM_ClearITPendingBit(TIM1, TIM_IT_CC3);     //���update�¼��жϱ�־
//		//TIM_SelectMasterSlaveMode(TIM1, TIM_MasterSlaveMode_Enable);//����ģʽMSM  
//		TIM_CtrlPWMOutputs(TIM1, ENABLE);
//		//TIM_SelectOnePulseMode(TIM1,TIM_OPMode_Single);

//		TIM_Cmd(TIM1, ENABLE);

		/* TIM1 ????? */
	
}

void TIM2_PWM_Config(void)
{
    TIM_TimeBaseInitTypeDef timer_init_structure;  
		TIM_OCInitTypeDef timer_OCinit_structure; 
    NVIC_InitTypeDef nvic_init_structure;  
  	GPIO_InitTypeDef gpio_init_structure;	

		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA , ENABLE);	
	
		nvic_init_structure.NVIC_IRQChannel = TIM2_IRQn;                //ʹ��TIM2�ж�ͨ��  
    nvic_init_structure.NVIC_IRQChannelCmd = ENABLE;                //ʹ��TIM2�ж�  
    nvic_init_structure.NVIC_IRQChannelPriority = 6;                //���ȼ�Ϊ0  
		
    NVIC_Init(&nvic_init_structure); 
		
    gpio_init_structure.GPIO_Pin = PWM1_Pin;  
    gpio_init_structure.GPIO_Mode = GPIO_Mode_AF;                   //????(??)??  
    gpio_init_structure.GPIO_Speed = GPIO_Speed_2MHz;              //Fast speed  
    gpio_init_structure.GPIO_PuPd= GPIO_PuPd_NOPULL;                    //??  
    GPIO_Init(PWM1_GPIO_Port, &gpio_init_structure);

		GPIO_PinAFConfig(GPIOA,GPIO_PinSource2,GPIO_AF_2);
		/*TIM2*/
    TIM_DeInit(TIM2);                                               //��λTIM2  
    TIM_TimeBaseStructInit(&timer_init_structure);                  //��ʼ��TIM�ṹ��  
  
    timer_init_structure.TIM_CounterMode = TIM_CounterMode_Up;      //���ϼ���ģʽ  
    timer_init_structure.TIM_Period = 960;                          //ÿ43 uS����һ���ж�,??ADC  
    timer_init_structure.TIM_Prescaler = 5;                      //����ʱ�ӷ�Ƶ,f=1M,systick=1 uS  
    timer_init_structure.TIM_RepetitionCounter = 0;              //����0+1��update�¼������ж� 
		
    TIM_TimeBaseInit(TIM2, &timer_init_structure);  
  
		timer_OCinit_structure.TIM_OCMode = TIM_OCMode_PWM1;
		timer_OCinit_structure.TIM_OutputState = TIM_OutputState_Enable;
		timer_OCinit_structure.TIM_Pulse = PWM1_HIGH;
		timer_OCinit_structure.TIM_OCPolarity = TIM_OCPolarity_High;

		TIM_OC3Init(TIM2,&timer_OCinit_structure);
		TIM_OC3PreloadConfig(TIM2,TIM_OCPreload_Enable);
		TIM_ARRPreloadConfig(TIM2,ENABLE);
		
    //TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);                      //ʹ��TIM2�ж�
		//TIM_SelectOutputTrigger(TIM2, TIM_TRGOSource_Update);           //ѡ��TIM1��updateΪ����Դ  
		//TIM_SelectInputTrigger(TIM2, TIM_TS_ITR0);
		//TIM_SelectSlaveMode(TIM2, TIM_SlaveMode_Gated);//����ģʽֻ�������ſ�����ͣ�����Կ���
		//TIM_SelectMasterSlaveMode(TIM2, TIM_MasterSlaveMode_Enable);//����ģʽMSM

		//TIM_Cmd(TIM2, ENABLE);
}

void TIM3_Config(void)
{
    TIM_TimeBaseInitTypeDef timer_init_structure;  
    NVIC_InitTypeDef nvic_init_structure;  
	
	  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);            //??TIM2??  //??GPIO??  
	
		nvic_init_structure.NVIC_IRQChannel = TIM3_IRQn;                //ʹ��TIM3�ж�ͨ��  
    nvic_init_structure.NVIC_IRQChannelCmd = ENABLE;                //ʹ��TIM3�ж�  
    nvic_init_structure.NVIC_IRQChannelPriority = 4;                //���ȼ�Ϊ4
		
    NVIC_Init(&nvic_init_structure); 

		/*TIM3*/
		TIM_DeInit(TIM3);                                               //��λTIM3
    TIM_TimeBaseStructInit(&timer_init_structure);                  //��ʼ��TIM�ṹ��  
  
    timer_init_structure.TIM_ClockDivision = TIM_CKD_DIV1;          //ϵͳʱ��,����Ƶ,24M  
    timer_init_structure.TIM_CounterMode = TIM_CounterMode_Up;      //���ϼ���ģʽ  
    timer_init_structure.TIM_Period = 6000;                          //ÿ1000 uS����һ���ж�,??ADC  
    timer_init_structure.TIM_Prescaler = 1;                      //����ʱ�ӷ�Ƶ,f=1M,systick=1 uS  
    timer_init_structure.TIM_RepetitionCounter = 0x00;              //����0+1��update�¼������ж� 
		
    TIM_TimeBaseInit(TIM3, &timer_init_structure);  
		//TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);                      //ʹ��TIM3�ж�
    TIM_Cmd(TIM3, ENABLE);                                          //ʹ��TIM3
	
}
    
/****************************??????****************************/  

void TIM2_IRQHandler()  
{  
    if(TIM_GetITStatus(TIM2, TIM_FLAG_Update))            //�жϷ���update�¼��ж�  
    {
				//_Gpio_7_set;
        TIM_ClearFlag(TIM2, TIM_FLAG_Update);     //���update�¼��жϱ�־
    }  
}  

void TIM1_BRK_UP_TRG_COM_IRQHandler()  
{  
    if(TIM_GetITStatus(TIM1, TIM_FLAG_Update)==SET)            //�жϷ���update�¼��ж�  
    {
			//_Gpio_7_set;
      TIM_ClearFlag(TIM1, TIM_FLAG_Update);     //���update�¼��жϱ�־
    }
//		if(TIM_GetITStatus(TIM1, TIM_IT_CC1) != RESET)
//		{
//			TIM_ClearFlag(TIM1, TIM_IT_CC1 ); //���update�¼��жϱ�־		
//		}
		if(TIM_GetITStatus(TIM1, TIM_IT_CC2) != RESET)
		{
			//ADC_StartOfConversion(ADC1);
			//_Gpio_7_set;
			TIM_ClearFlag(TIM1, TIM_FLAG_CC2);     //���update�¼��жϱ�־
		}
//		if(TIM_GetITStatus(TIM1, TIM_IT_CC3) != RESET)
//		{
//			TIM_ClearFlag(TIM1, TIM_FLAG_CC3);     //���update�¼��жϱ�־
//		}
} 



extern uint32_t   ShortCircuitLastTime;
void TIM3_IRQHandler()
{
	  if(TIM_GetITStatus(TIM3, TIM_IT_Update))            //�жϷ���update�¼��ж�  
    { 
//			scan_tick++;
			_Gpio_7_set;
//			if(scan_tick>=10)  /*10*1.22=12.2ms*/
//			{
//				scan_tick = 0;
//				scan_key();
//			}
			TIM_ClearITPendingBit(TIM3, TIM_IT_Update);     //���update�¼��жϱ�־
		}
}


int16_t  RunTime = 0; 

extern uint8_t SelfGetADCWell;
extern uint8_t FB_Flag;
uint8_t ADCIndex=0;
uint8_t DMAIndex=0;
extern uint32_t selfADCValue;
uint8_t ADC_Conversion_Flag=0;
void DMA1_Channel1_IRQHandler()  
{  
    if(DMA_GetITStatus(DMA_IT_TC))                      //�ж�DMA��������ж�  
    {   
			selfADCValue  = adc_dma_tab[0];
			//_Gpio_7_set;
			ADC_Conversion_Flag=1;
			DMA_ClearITPendingBit(DMA_IT_TC);                   //���DMA�жϱ�־λ  
    }  
}  

void TIM14_IRQHandler()
{
	  if(TIM_GetITStatus(TIM14, TIM_IT_Update))            //�жϷ���update�¼��ж�  
    { 
			scan_tick++;
			//_Gpio_7_set;
			if(scan_tick>=10)  /*10*1.22=12.2ms*/
			{
				scan_tick = 0;
				scan_key();
			}
			TIM_ClearITPendingBit(TIM14, TIM_IT_Update);     //���update�¼��жϱ�־
		}
}
/***********************/
void TIM14_Config(void)
{
    TIM_TimeBaseInitTypeDef timer_init_structure;  
		NVIC_InitTypeDef nvic_init_structure;  	
	
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM14, ENABLE);
		/*TIM14*/
		TIM_DeInit(TIM14);                                               //��λTIM14

		nvic_init_structure.NVIC_IRQChannel = TIM14_IRQn;                //ʹ��TIM14�ж�ͨ��  
    nvic_init_structure.NVIC_IRQChannelCmd = ENABLE;                //ʹ��TIM14�ж�  
    nvic_init_structure.NVIC_IRQChannelPriority = 1;                //���ȼ�Ϊ4
    NVIC_Init(&nvic_init_structure); 	

    TIM_TimeBaseStructInit(&timer_init_structure);                  //��ʼ��TIM�ṹ��  
  
    timer_init_structure.TIM_ClockDivision = TIM_CKD_DIV1;          //ϵͳʱ��,����Ƶ,24M  
    timer_init_structure.TIM_CounterMode = TIM_CounterMode_Up;      //���ϼ���ģʽ  
    timer_init_structure.TIM_Period = 1000;                          //ÿ300 uS����һ���ж�,??ADC  
    timer_init_structure.TIM_Prescaler = 23;                      //����ʱ�ӷ�Ƶ,f=1M,systick=1 uS  
    timer_init_structure.TIM_RepetitionCounter = 0x00;              //����0+1��update�¼������ж� 
		
    TIM_TimeBaseInit(TIM14, &timer_init_structure);  
		//TIM_ITConfig(TIM14, TIM_IT_Update, ENABLE);                      //ʹ��TIM14�ж�
    //TIM_Cmd(TIM14, ENABLE);                                          //ʹ��TIM14
}


void GPIO_INIT(void)
{
	
	    GPIO_InitTypeDef gpio_init_structure;  
    //??GPIO??  
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA|RCC_AHBPeriph_GPIOB|RCC_AHBPeriph_GPIOF, ENABLE);  
  
    GPIO_StructInit(&gpio_init_structure);  
                                                        //PA-0~3??ADC  
    gpio_init_structure.GPIO_Pin = OUT_Pin;  
    gpio_init_structure.GPIO_Mode = GPIO_Mode_OUT;                   //????(??)??  
    gpio_init_structure.GPIO_OType = GPIO_OType_PP;                 //????  
    gpio_init_structure.GPIO_Speed = GPIO_Speed_2MHz;              //Fast speed  
    gpio_init_structure.GPIO_PuPd= GPIO_PuPd_NOPULL;                    //??  
    GPIO_Init(OUT_GPIO_Port, &gpio_init_structure);  
	
		gpio_init_structure.GPIO_Pin = SW1_LD_Pin;
		gpio_init_structure.GPIO_Mode = GPIO_Mode_IN;                   //????(??)??  
    gpio_init_structure.GPIO_Speed = GPIO_Speed_2MHz;              //Fast speed  
		gpio_init_structure.GPIO_PuPd= GPIO_PuPd_UP;                    //??
    GPIO_Init(SW1_LD_GPIO_Port, &gpio_init_structure); 
	
		gpio_init_structure.GPIO_Pin = SW1_ONOFF_Pin;
		gpio_init_structure.GPIO_Mode = GPIO_Mode_IN;                   //????(??)??  
    gpio_init_structure.GPIO_Speed = GPIO_Speed_2MHz;              //Fast speed  
		gpio_init_structure.GPIO_PuPd= GPIO_PuPd_UP;                    //??
    GPIO_Init(SW1_ONOFF_GPIO_Port, &gpio_init_structure);  
	
    gpio_init_structure.GPIO_Pin = ALM_Pin;  
    gpio_init_structure.GPIO_Mode = GPIO_Mode_OUT;                   //????(??)??  
    gpio_init_structure.GPIO_OType = GPIO_OType_PP;                 //????  
    gpio_init_structure.GPIO_Speed = GPIO_Speed_2MHz;              //Fast speed  
    gpio_init_structure.GPIO_PuPd= GPIO_PuPd_NOPULL;                    //??  
    GPIO_Init(ALM_GPIO_Port, &gpio_init_structure);  
	
		gpio_init_structure.GPIO_Pin = SC_Pin;
		gpio_init_structure.GPIO_Mode = GPIO_Mode_IN;                   //????(??)??  
    gpio_init_structure.GPIO_Speed = GPIO_Speed_2MHz;              //Fast speed  
		gpio_init_structure.GPIO_PuPd= GPIO_PuPd_UP;                    //??
    GPIO_Init(SC_GPIO_Port, &gpio_init_structure);  

}

void GPIO_DEINIT_ALL(void)
{
	
	   GPIO_InitTypeDef gpio_init_structure;  
    //??GPIO??  
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA|RCC_AHBPeriph_GPIOB|RCC_AHBPeriph_GPIOF, ENABLE);  
  
    GPIO_StructInit(&gpio_init_structure);  
                                                        //PA-0~3??ADC  
    gpio_init_structure.GPIO_Pin = GPIO_Pin_7;  
    gpio_init_structure.GPIO_Mode = GPIO_Mode_OUT;                   //????(??)??  
    gpio_init_structure.GPIO_OType = GPIO_OType_PP;                 //????  
    gpio_init_structure.GPIO_Speed = GPIO_Speed_2MHz;              //Fast speed  
    gpio_init_structure.GPIO_PuPd= GPIO_PuPd_DOWN;                    //??  
    GPIO_Init(GPIOA, &gpio_init_structure); 
	
}


void RCC_Configuration(void)
{
	RCC_PLLConfig(RCC_PLLSource_HSI,RCC_PLLMul_6);
	RCC_PLLCmd(ENABLE);
	RCC_ADCCLKConfig(RCC_ADCCLK_PCLK_Div2);
	while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY)==RESET)
	{
		
	}
	
	RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
	
	while(RCC_GetSYSCLKSource()!=0x08)
	{
	}
}

/**
  * @brief Configures EXTI Lines.
  * @param None
  * @retval None
  */
void EXTI_Configuration(void)
{
  EXTI_InitTypeDef EXTI_InitStructure;

  /* Configure EXTI Line16(PVD Output) to generate an interrupt on rising and
     falling edges */
  EXTI_ClearITPendingBit(EXTI_Line16); 
  EXTI_InitStructure.EXTI_Line = EXTI_Line16;// PVD���ӵ��ж�16���� 
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;//ʹ���ж�ģʽ
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;//��ѹ�½����趨��ֵʱ�����ж�
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;// ʹ���ж�
  EXTI_Init(&EXTI_InitStructure);// ��ʼ��
}

/**
  * @brief Configures NVIC and Vector Table base location.
  * @param None
  * @retval None
  */
void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  
  /* Configure one bit for preemption priority */
  //NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);//�ж����ȼ�����
  
  /* Enable the PVD Interrupt */ //����PVD�ж�
  NVIC_InitStructure.NVIC_IRQChannel = PVD_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;                //ʹ��PVD�ж�  
	NVIC_InitStructure.NVIC_IRQChannelPriority = 10;                //���ȼ�Ϊ10
  NVIC_Init(&NVIC_InitStructure);
}

void PVD_init(void)
{
	EXTI_Configuration();
	
	NVIC_Configuration();
	
	//RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
	PWR_PVDLevelConfig(PWR_PVDLevel_7); /*����PVD��ѹ���*/
	PWR_PVDCmd(ENABLE);
}


void DelaymsSet(int16_t ms)
{
		while(1)
		{
			ms--;
			if(ms<=0)
				break;
		}
}

uint8_t CheckFLag=0;

/*************************************************************/
#define 	FLASH_START_ADDR1 	 						0x08007000    //0x0800FFF0

uint8_t* UID=(uint8_t*)0x1FFFF7AC;  //��ȡUID  stm32f0:0x1FFFF7AC,stm32f100:0x1FFFF7E8
uint32_t Fml_Constant 	= 0x19101943;//���뵽��ʽ�ĳ���
uint8_t *C= (uint8_t*)&Fml_Constant;//�ѳ���ת��������
uint8_t FormulaResult[4];
uint32_t FormulaCheck;
uint32_t UIDFlashResult;
uint16_t Fml_CRC16;
uint8_t D[12];
int ProgramCounter=0;

void Formula_100(uint8_t *D,uint8_t *Result)
{
	D[0] = UID[4];
	D[1] = UID[8];
	D[2] = UID[1];
	D[3] = UID[3];
	D[4] = UID[0];
	D[5] = UID[5];
	D[6] = UID[10];
	D[7] = UID[7];
	D[8] = UID[9];
	D[9] = UID[2];
	D[10] = UID[11];
	D[11] = UID[6];
	
	Result[0] = C[0] ^ D[0];
	Result[1] = C[1] ^ D[6] ^ D[7] ^ D[8] ^ D[9] ^ D[10] ^ D[11] ;
	Result[2] = C[2] ^ D[4] ;
	Result[3] = C[3] ^ D[2] ^ D[1];
}

uint8_t FlashCheck(void)
{
		uint8_t FlashFlag;
		Formula_100(D,FormulaResult);
		FormulaCheck = FormulaResult[0]+(FormulaResult[1]<<8)+(FormulaResult[2]<<16)+(FormulaResult[3]<<24);
		//UIDFlashResult = *(__IO uint32_t*)(FLASH_START_ADDR1);
		UIDFlashResult = *(uint32_t*)(FLASH_START_ADDR1);
		if(UIDFlashResult==FormulaCheck)
			FlashFlag =1;
		else
			FlashFlag =0;
		
		return FlashFlag;
}

uint16_t Formula_CRC16(uint8_t *p,uint8_t len)
{
	uint8_t i;
	while(len--)
	{
		for(i=0x80; i!=0; i>>=1)
		{
			if((Fml_CRC16 & 0x8000) != 0)
			{
				Fml_CRC16 <<= 1;
				Fml_CRC16 ^= 0x1021;
			}
			else
			{
				Fml_CRC16 <<= 1;
			}
			if((*p&i)!=0)
			{
				Fml_CRC16 ^= 0x1021;
			}
		}
		p++;
	}
	return Fml_CRC16;
}
//void ProgramCheck(void)
//{
//	ProgramCounter 		= ReadFlash(ProgramRUNcounter_Mode_FLASH_DATA_ADDRESS);
//	if(ProgramCounter>65535 || ProgramCounter<0)
//	{
//		ProgramCounter = 0;
//		ResetParameter();
//	}
//	ProgramCounter = ProgramCounter+1;
//	WriteFlash(ProgramRUNcounter_Mode_FLASH_DATA_ADDRESS,ProgramCounter);
//	DelaymsSet(50); 	
//	if(ProgramCounter<=1)
//	{
//		ResetParameter();
//	}
//}
void ProgramCheck(void)
{
	ProgramCounter 		= *(__IO uint32_t*)(ProgramRUNcounter_Mode_FLASH_DATA_ADDRESS);
	if(ProgramCounter == 0xFFFFFFFF)
	{
		ResetParameter();
		WriteFlash(ProgramRUNcounter_Mode_FLASH_DATA_ADDRESS, 0x0505);
	}
	DelaymsSet(50);
}



void IWDG_Config(void)
{
	if(RCC_GetFlagStatus(RCC_FLAG_IWDGRST) != RESET)
	{
		RCC_ClearFlag();
	}
	
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
	IWDG_SetPrescaler(IWDG_Prescaler_64);
	IWDG_SetReload(40000/20);
	IWDG_ReloadCounter();
	
	IWDG_Enable();
}

void PVD_IRQHandler(void)
{
//	if(EXTI_GetITStatus(EXTI_Line16) != RESET)
//	{
//		PVD_Flag = 1;
//		/* Clear the Key Button EXTI line pending bit */
//		EXTI_ClearITPendingBit(EXTI_Line16);
//	}
	if(PWR_GetFlagStatus(PWR_FLAG_PVDO)!=RESET)
	{
		PVD_Flag = 1;
	}
	else
	{
		PVD_Flag = 0;
	}
}

/***************������***************/
/*SYS Clokc 24MHz ADC clock 12MHz*/
int main(void)
{
		uint32_t checkcouter;
		RCC_Configuration();
		GPIO_INIT();
		BSP_init();
		RCC_GetClocksFreq(&SysClock);
		DelaymsSet(5000); 
		IWDG_Config();
		CheckFLag = FlashCheck();
	  
	  
		if(CheckFLag)
		//if(1)
		{
			/*�������д������*/
			ProgramCheck();
			/*��Ҫ���к���*/
			DataProcess();
		}
		else
			while(1)
			{
				checkcouter++;
			}	
}
