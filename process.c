///**
//  ********************************  STM32F0xx  *********************************
//  * @�ļ���     �� process.c
//  * @����       �� HarryZeng
//  * @��汾     �� V1.5.0
//  * @�ļ��汾   �� V1.0.0
//  * @����       �� 2017��04��21��
//  * @ժҪ       �� ���ݴ���
//  ******************************************************************************/
///*----------------------------------------------------------------------------
//  ������־:
//  2017-04-21 V1.0.0:��ʼ�汾
//  ----------------------------------------------------------------------------*/
///* ������ͷ�ļ� --------------------------------------------------------------*/
/* Includes ------------------------------------------------------------------*/
#include "process.h"
#include "flash.h"
/*----------------------------------�궨��-------------------------------------*/


/*------------------------------------��������---------------------------------------*/
void 	MARK_GetRegisterAState(void);
void 	CI_GetRegisterAState(void);
uint32_t 	Read_Value(PWM_Number PWM);
uint8_t  	Read_GOODBAD(void);
void  SetOut(uint8_t OUT_Value,uint8_t DelyaStatus);
void  GetRegisterAState(void);
//void  MARK_Mode_SelfLearning(void);
void 	scan_key(void);
void 	printFlashTest(void);
void 	ShortCircuitProtection(void);
void 	WriteFlash(uint32_t addr,uint32_t data);
extern void DelaymsSet(int16_t ms);
void SET_GOODBAD(void);
void GetEEPROM(void);
void LOW_PWM_OUT(void);
void HIGH_PWM_OUT(void);
uint8_t Get_FB_Flag(void);
void SelfLearning(void);
/*------------------------------------ȫ�ֱ���---------------------------------------*/
uint32_t  ADC_value[8]={0,0,0,0,0,0,0,0};
uint32_t  ADC_SumValue;

uint8_t 	ShortCircuit=0;
uint8_t 	ConfirmShortCircuit=0;
uint32_t 		ShortCircuitTimer=0;
uint32_t   ShortCircuitCounter = 0;
uint32_t   ShortCircuitLastTime = 0;
/*״̬����*/
uint8_t RegisterA = 0;
uint8_t RegisterB = 0;
uint8_t RegisterA_Comfirm = 0;
uint8_t OUT;

	uint32_t S[2];

	float SXA_B[2],SYA_B[2],SZA_B[2];

	float X=0,Y=0,Z=0,BIG=0;
	uint32_t SelfLADC=0;
	uint8_t SelfGetADCWell=0;
	uint32_t temppp;
	extern uint8_t DMAIndex;
uint32_t selfADCValue;

PWM_Number CurrentPWM = PWMX; //Ĭ�ϵ�ǰPWMͨ��ΪX
uint32_t S_Last,S_Current,S_History,S_FINAL;
int CICurrentThreshold=500;
int MAKCurrentThreshold=500;

uint32_t RegisterACounter=0;

uint8_t KeyTime;
uint16_t key_counter=0;
uint16_t scan_tick=0;
uint8_t KeyIndex=0;
uint32_t FLASHData;
uint8_t EnterSelfFlag=0;
extern int16_t adc_dma_tab[6];
extern uint8_t sample_finish;
extern uint8_t TIM1step;

/********************************/
int32_t DX_Data[8];
int16_t DX_Max = 0,DX_Min=4095;
uint8_t DX_Index = 0;
int DX=0;

int32_t DX2_Data[4];
int16_t DX2_Max = 0,DX2_Min=4095;
uint8_t DX2_Index = 0;
int DX2=0;

uint16_t  GoodBadTime=0;

extern uint8_t ADC_Conversion_Flag;
uint8_t PVD_Flag = 0;
/***********************************/
/*----------------------------------��������----------------------------------------*/
/*****************
*
*�����ݴ�������
*
*****************/
void DataProcess(void)
{
	int First_ten_times;

	/*
		FALSH ��ȡ����
	*/
	//GetEEPROM();
	
	for(First_ten_times = 0;First_ten_times<20;First_ten_times++) /*���ϵ磬ǰʮ��PWMֻ�� RegisterA*/
	{
			//LOW_PWM_OUT();
			IWDG_ReloadCounter();//���Ź�ι��
			while(TIM_GetCounter(MainTIMER)<PWM1_HIGH){}//һ���ۼ���ɣ��ȴ�
	}
	
	while(1)
	{
		if(PVD_Flag==0)
			GetRegisterAState();		
	}
}

/***************************************
*
*����OUT�������ƽ
*
**************************************/
uint16_t OffDelayTime=0;
uint16_t OffDelayFlag=0;
void  SetOut(uint8_t OUT_Value,uint8_t OffDelayCmd)
{
	if(OffDelayCmd)  /*OFF DELAY enable*/
	{
		if(OUT_Value==1&&OffDelayFlag==0)
		{
			GPIO_WriteBit(OUT_GPIO_Port, OUT_Pin, Bit_SET);
			//_Gpio_7_set;
		}
		else
		{
			OffDelayFlag = 1;
			OffDelayTime++;
			//_Gpio_7_set;
			if(OffDelayTime>220)//10ms
			{
				GPIO_WriteBit(OUT_GPIO_Port, OUT_Pin, Bit_RESET);
				OffDelayFlag = 0;
				OffDelayTime = 0;
				//_Gpio_7_reset;
			}
		}
	}
	else /*OFF DELAY disable*/
	{
		if(OUT_Value==1)
		{
			GPIO_WriteBit(OUT_GPIO_Port, OUT_Pin, Bit_SET);
		}
		else
		{
			GPIO_WriteBit(OUT_GPIO_Port, OUT_Pin, Bit_RESET);
		}
	}
}



 void PWM_LowUs_Set(void)  //1.24us
 {
		/*����*/
		GPIOA->BSRR = 0x0001;
		GPIOA->BSRR = 0x0001;
		GPIOA->BSRR = 0x0001;
		GPIOA->BSRR = 0x0001;
		GPIOA->BSRR = 0x0001;
		GPIOA->BSRR = 0x0001;
		GPIOA->BSRR = 0x0001;
		GPIOA->BSRR = 0x0001;
		GPIOA->BSRR = 0x0001;
		GPIOA->BSRR = 0x0001;
		GPIOA->BSRR = 0x0001;
		GPIOA->BSRR = 0x0001;
		GPIOA->BSRR = 0x0001;
		GPIOA->BSRR = 0x0001;
		/*����*/
		GPIOA->BRR = 0x0001;
 }
 

void PWM_HighUs_Set(void)  //3us
 {
		/*����*/
		GPIOA->BSRR = 0x0001;
		GPIOA->BSRR = 0x0001;
		GPIOA->BSRR = 0x0001;
		GPIOA->BSRR = 0x0001;
		GPIOA->BSRR = 0x0001;
		GPIOA->BSRR = 0x0001;
		GPIOA->BSRR = 0x0001;
		GPIOA->BSRR = 0x0001;
		GPIOA->BSRR = 0x0001;
		GPIOA->BSRR = 0x0001;
		GPIOA->BSRR = 0x0001;
		GPIOA->BSRR = 0x0001;
		GPIOA->BSRR = 0x0001;
		GPIOA->BSRR = 0x0001;
		GPIOA->BSRR = 0x0001;
		GPIOA->BSRR = 0x0001;
		GPIOA->BSRR = 0x0001;
		GPIOA->BSRR = 0x0001;
		GPIOA->BSRR = 0x0001;
		GPIOA->BSRR = 0x0001;
		GPIOA->BSRR = 0x0001;
		GPIOA->BSRR = 0x0001;
		GPIOA->BSRR = 0x0001;
		GPIOA->BSRR = 0x0001;
		GPIOA->BSRR = 0x0001;
		GPIOA->BSRR = 0x0001;
		GPIOA->BSRR = 0x0001;
		GPIOA->BSRR = 0x0001;
		GPIOA->BSRR = 0x0001;
		GPIOA->BSRR = 0x0001;
		GPIOA->BSRR = 0x0001;
		GPIOA->BSRR = 0x0001;
		GPIOA->BSRR = 0x0001;
		GPIOA->BSRR = 0x0001;
		GPIOA->BSRR = 0x0001;
		/*����*/
		GPIOA->BRR = 0x0001;
 }
 
void PWM_OUT(void)
{
	uint8_t adc_transform_counter=0;
	TIM_SetCounter(MainTIMER,0X00);
	/******************************S1********************************/
	while(adc_transform_counter<8)
	{
		ADC1->CR|=1<<2;          
		while(((ADC1->CR)&0x0010)== 1){}
		ADC_value[adc_transform_counter++] =  ADC1->DR;   
	}
	while(TIM_GetCounter(MainTIMER)<StartTime);// �ȴ����Ƿ����50us
	PWM_HighUs_Set();	
	//_Gpio_7_set;
}


uint8_t GetRegisterBStatus(void)
{
	if(GPIO_ReadInputDataBit(SW1_LD_GPIO_Port,SW1_LD_Pin) == Bit_RESET)
		return 0;
	else if(GPIO_ReadInputDataBit(SW1_LD_GPIO_Port,SW1_LD_Pin) == Bit_SET)
		return 1;
}
uint8_t GetNOOFFStatus(void)
{
	if(GPIO_ReadInputDataBit(SW1_ONOFF_GPIO_Port,SW1_ONOFF_Pin) == Bit_RESET)
		return 0;
	else if(GPIO_ReadInputDataBit(SW1_ONOFF_GPIO_Port,SW1_ONOFF_Pin) == Bit_SET)
		return 1;
}

void SetALM(uint8_t ALMStatus)
{
	if(ALMStatus)
		GPIO_WriteBit(ALM_GPIO_Port,ALM_Pin,Bit_SET);

	else 
		GPIO_WriteBit(ALM_GPIO_Port, ALM_Pin, Bit_RESET);
}

/********************
*
*�ж�RegisterA״̬
*
**********************/
extern uint8_t ADCIndex;
uint8_t RegisterA_temp=0;
uint8_t RegisterA_1Counter = 0;
uint8_t RegisterA_0Counter = 0;
uint32_t S_Final=0;
uint8_t DelayONOFF=0;
uint8_t ALMFlag=0;
void GetRegisterAState(void)
{
	uint8_t i=0;
	PWM_OUT();
	for(i=0;i<8;i++)
		ADC_SumValue = ADC_value[i] + ADC_SumValue;
	S_Final=0;
	S_Final = ADC_SumValue/8;
	ADC_SumValue = 0;
	
	if(S_Final>=2600)
	{
		RegisterA_temp = 0;
		RegisterA_0Counter++;
		RegisterA_1Counter = 0;
	}
	else if(S_Final<=2500)
	{
		RegisterA_temp = 1;
		RegisterA_1Counter++;
		RegisterA_0Counter = 0;
	}
		/*�ж�״̬�����ķ�Χ�����RegisterA��ֵ*/
	if(RegisterA_0Counter>3)
	{
		RegisterA = 0;
		RegisterA_0Counter = 0;
	}
	else if(RegisterA_1Counter>3)
	{
		RegisterA = 1;
		RegisterA_1Counter = 0;
	}
	/*ALM�ж�*/
	if(S_Final<=2650)
		ALMFlag=1;
	else if(S_Final>=2700)
		ALMFlag=0;
	
//	if(X_Index>3)
//	{
//		X1 = X_1 / 4;
//		X2 = X_2 / 4;
//		X_1 = 0;
//		X_2 = 0;
//		X_Final =  X1 - X2;
//		if(X_Final<0)
//			X_Final = 0;
//	}
//	
//			DX_Data[DX_Index] = X_Final;
//			if(DX_Data[DX_Index]>DX_Max)
//				DX_Max = DX_Data[DX_Index];
//			if(DX_Data[DX_Index] < DX_Min)
//				DX_Min = DX_Data[DX_Index];
//			DX_Index++;
//			if(DX_Index>5)  //����6��X_Final�����ֵ����Сֵ�Ĳ�ֵ
//			{
//				DX_Index = 0;
//				DX = DX_Max - DX_Min;
//				DX_Max = 0;
//				DX_Min = 4095;
//			}
			/***********RegisterA***********/
		
//			X_Final_Max = 40 + 0.5*DX; 
//			X_Final_Min = 20 - 0.5*DX;
//		
//			if(X_Final_Min<0)
//				 X_Final_Min= 0;	

		/*���ALM*/
		SetALM(ALMFlag);
		/*ͬ������*/
		RegisterB = GetRegisterBStatus();
		OUT=!(RegisterB^RegisterA);
		/*��ȡ����Ƿ���ʱ״̬*/
		DelayONOFF = GetNOOFFStatus();
		/*���OUT*/
		SetOut(OUT,DelayONOFF);
	/*******************************************/
		IWDG_ReloadCounter();//���Ź�ι��
		//_Gpio_7_set;
}

/***************************************
*
*ɨ�谴��ʱ��
*
**************************************/
void scan_key(void) 
{ 
//	if(SETPin==Bit_SET )
//	{
//			key_counter++;
//	}
//	
//	else	if (key_counter>middleKEY) 
//		{ 
//				KeyTime = key_counter; 
//				key_counter = 0;
//		}
//	 else if(key_counter<middleKEY && key_counter>shortKEY)
//			{ 
//					KeyTime = key_counter; 
//					key_counter = 0;
//			}
//	else	if(key_counter<shortKEY&&key_counter>2)
//		{ 
//				KeyTime = key_counter;  
//				key_counter = 0;
//		}
			
}

//FLASH��ȡ���ݲ���
/*****************************
*
*��ʼ�����в���
*
****************************/
void ResetParameter(void)
{
//		MAKCurrentThreshold=500;
//		CurrentPWM = PWMX;
//	
//		CICurrentThreshold=500;
//		SA_B[0] = 0x00;
//		CXA_B[0] = 0x00;
//		CYA_B[0] = 0x00;
//		CZA_B[0] = 0x00;
//		
//		WriteFlash(MAKCurrentThreshold_FLASH_DATA_ADDRESS,MAKCurrentThreshold);

//		WriteFlash(CurrentPWM_FLASH_DATA_ADDRESS,CurrentPWM);

//		WriteFlash(CICurrentThreshold_FLASH_DATA_ADDRESS,CICurrentThreshold);

//		WriteFlash(SA_FLASH_DATA_ADDRESS,SA_B[0]);

//		WriteFlash(CXA_FLASH_DATA_ADDRESS,CXA_B[0]);

//		WriteFlash(CYA_FLASH_DATA_ADDRESS,CYA_B[0]);

//		WriteFlash(CZA_FLASH_DATA_ADDRESS,CZA_B[0]);
}

void GetEEPROM(void)
{
//			MAKCurrentThreshold 	= ReadFlash(MAKCurrentThreshold_FLASH_DATA_ADDRESS);
//			CurrentPWM 						= ReadFlash(CurrentPWM_FLASH_DATA_ADDRESS);
//			CICurrentThreshold 		= ReadFlash(CICurrentThreshold_FLASH_DATA_ADDRESS);
//			SA_B[0] 							= ReadFlash(SA_FLASH_DATA_ADDRESS);
//			CXA_B[0] 							= ReadFlash(CXA_FLASH_DATA_ADDRESS);
//			CYA_B[0] 							= ReadFlash(CYA_FLASH_DATA_ADDRESS);
//			CZA_B[0] 							= ReadFlash(CZA_FLASH_DATA_ADDRESS);

}

/*******************************
*
*��·����
*
*******************************/
void ShortCircuitProtection(void)
{
//	uint8_t SCState;
//	
//	/*��ȡSC���ŵ�״̬*/
//	if(ShortCircuit!=1)
//	{
//		SCState = GPIO_ReadInputDataBit(SC_GPIO_Port ,SC_Pin);
//		if(SCState == Bit_RESET)
//		{
//			/*����FB_SC*/
//			ShortCircuit= 1;
//		}
//		else
//		{
//			ShortCircuit = 0;
//			ConfirmShortCircuit = 0;
//		}
//	}
//	if(ShortCircuit && ShortCircuitCounter>=5)
//	{
//		ConfirmShortCircuit=1;
//		
//		GPIO_WriteBit(OUT_GPIO_Port, OUT_Pin, Bit_RESET);/*��������OUT*/
//		ShortCircuitTimer = ShortCircuitLastTime;
//	}
}

///**** Copyright (C)2017 HarryZeng. All Rights Reserved **** END OF FILE ****/