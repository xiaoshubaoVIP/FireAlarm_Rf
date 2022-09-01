#include "../inc/hal_rtc.h"
#include "ddl.h"

/**********************************************************************/
//Description:	DecToBcdConversion()															  
//Parameters:                        												  
//Return:   																  
//Date:           
//Author:       quanwu.xu  
/**********************************************************************/
static void DecToBcdConversion(stc_rtc_time_t* pTime)
{
    pTime->u8Second = ((((pTime->u8Second)/10)&0x07)<<4) + (((pTime->u8Second)%10)&0x0F);
    pTime->u8Minute = ((((pTime->u8Minute)/10)&0x07)<<4) + (((pTime->u8Minute)%10)&0x0F);
    pTime->u8Hour = ((((pTime->u8Hour)/10)&0x03)<<4) + (((pTime->u8Hour)%10)&0x0F);
    pTime->u8Day = ((((pTime->u8Day)/10)&0x03)<<4) + (((pTime->u8Day)%10)&0x0F);   
    pTime->u8DayOfWeek = (pTime->u8DayOfWeek)&0x07;     
    pTime->u8Month = ((((pTime->u8Month)/10)&0x01)<<4) + (((pTime->u8Month)%10)&0x0F);
    pTime->u8Year = ((((pTime->u8Year)/10)&0x0F)<<4) + (((pTime->u8Year)%10)&0x0F); 
}


static void RtcAlarmCb(void)
{
	
}

extern void ReadRtcTime(void);
void HalRtcInit(void)
{
	stc_rtc_config_t	stcRtcConfig; 
	stc_rtc_irq_cb_t	stcIrqCb;
	stc_rtc_time_t		stcTime;
	stc_rtc_alarmset_t	stcAlarm;
	stc_rtc_cyc_sel_t	stcCycSel;
	
	DDL_ZERO_STRUCT(stcRtcConfig);
	DDL_ZERO_STRUCT(stcIrqCb);
	DDL_ZERO_STRUCT(stcAlarm);
	DDL_ZERO_STRUCT(stcTime);
	DDL_ZERO_STRUCT(stcCycSel);

	
	Clk_Enable(ClkRCL, TRUE);							//inter 32k  
	Clk_SetPeripheralGate(ClkPeripheralRtc,TRUE);		//enable rtc clk
	
	stcRtcConfig.enClkSel = RtcClk32;					//RtcClk32;
	stcRtcConfig.enAmpmSel = Rtc24h;					//Rtc24h;
   
	stcCycSel.enCyc_sel = RtcPrads; 					//Mon, day, hour/min/sec						
	stcCycSel.enPrds_sel = Rtc_1Min;					//interrupt cycle
		
	stcRtcConfig.pstcCycSel = &stcCycSel;

	stcTime.u8Year = 0x19;
	stcTime.u8Month = 0x09;
	stcTime.u8Day = 0x01;
	stcTime.u8Hour = 0x08;								//10:24:15
	stcTime.u8Minute = 0x08;							
	stcTime.u8Second = 0x08;
	stcTime.u8DayOfWeek = Rtc_CalWeek(&stcTime.u8Day);

	DecToBcdConversion(&stcTime);	
	stcRtcConfig.pstcTimeDate = &stcTime;
	
	stcIrqCb.pfnAlarmIrqCb = RtcAlarmCb;
	stcIrqCb.pfnTimerIrqCb = ReadRtcTime;//RtcCycCb;
	stcRtcConfig.pstcIrqCb = &stcIrqCb;
	stcRtcConfig.bTouchNvic = TRUE;

	Rtc_EnAlarmIrq(Rtc_AlarmInt_Disable);
	
	Rtc_DisableFunc(RtcCount);
	
	Rtc_Init(&stcRtcConfig); 
	Rtc_EnableFunc(RtcCount);
}

