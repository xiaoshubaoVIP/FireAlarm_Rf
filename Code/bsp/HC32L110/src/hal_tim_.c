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

	Clk_SetPeripheralGate(ClkPeripheralBt,TRUE);	//开启时钟

	BaseTime1Buf.enGateP=BtPositive;                //门控电平选择		
	BaseTime1Buf.enGate=BtGateDisable;		        //禁止门控
	BaseTime1Buf.enPRS=BtPCLKDiv1;		            //不分频, 16MHz
	BaseTime1Buf.enTog=BtTogDisable;	  		    //翻转输出禁止
	BaseTime1Buf.enCT=BtTimer;				        //定时器功能Mode2
	BaseTime1Buf.enMD=BtMode2;					    //16位自动重载定时
	BaseTime1Buf.pfnTim1Cb=Time1IrqCb;		
	Bt_Init(TIM1,&BaseTime1Buf);

	Bt_Cnt16Set(TIM1,(65536-15957));		        //初值
	Bt_ARRSet(TIM1,(0));				    		//重载值
	Bt_ClearIntFlag(TIM1);					        //清标志
	Bt_EnableIrq(TIM1);					            //开启中断
	EnableNvic(TIM1_IRQn,3,TRUE);
	Bt_Run(TIM1);					                //启动
}


