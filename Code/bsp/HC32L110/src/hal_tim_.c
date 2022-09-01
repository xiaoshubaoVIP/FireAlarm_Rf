#include "../inc/hal_tim.h"
#include "ddl.h"

/**********************************************************************/
//Description:	HalGetTimCnt()											
//Parameters:                        									
//Return:   															
//Date:           
//Author:		quanwu.xu  
/**********************************************************************/
uint16_t HalGetTimCnt(tim_type Dev)
{
	uint16_t TimCnt = 0;
	Dev = Dev;

	TimCnt = Bt_Cnt16Get(TIM1);

	return TimCnt;
}

/**********************************************************************/
//Description:	Time1IrqCb()											
//Parameters:                        									
//Return:   															
//Date:           
//Author:		quanwu.xu  
/**********************************************************************/
void Time1IrqCb(void)
{
	Bt_ClearIntFlag(TIM1);
}

/**********************************************************************/
//Description:	Time1Init()												
//Parameters:                        									
//Return:   															
//Date:           
//Author:		quanwu.xu  
/**********************************************************************/
void HalTimInit(tim_type Dev)
{
	stc_bt_config_t BaseTime1Buf;

	Dev = Dev;

	Clk_SetPeripheralGate(ClkPeripheralBt,TRUE);	//����ʱ��

	BaseTime1Buf.enGateP=BtPositive;                //�ſص�ƽѡ��		
	BaseTime1Buf.enGate=BtGateDisable;		        //��ֹ�ſ�
	BaseTime1Buf.enPRS=BtPCLKDiv1;		            //����Ƶ, 16MHz
	BaseTime1Buf.enTog=BtTogDisable;	  		    //��ת�����ֹ
	BaseTime1Buf.enCT=BtTimer;				        //��ʱ������Mode2
	BaseTime1Buf.enMD=BtMode2;					    //16λ�Զ����ض�ʱ
	BaseTime1Buf.pfnTim1Cb=Time1IrqCb;		
	Bt_Init(TIM1,&BaseTime1Buf);

	Bt_Cnt16Set(TIM1,(65536-15957));		        //��ֵ
	Bt_ARRSet(TIM1,(0));				    		//����ֵ
	Bt_ClearIntFlag(TIM1);					        //���־
	Bt_EnableIrq(TIM1);					            //�����ж�
	EnableNvic(TIM1_IRQn,3,TRUE);
	Bt_Run(TIM1);					                //����
}


