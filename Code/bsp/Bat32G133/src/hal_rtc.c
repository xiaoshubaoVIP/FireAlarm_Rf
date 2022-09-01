#include "../inc/hal_rtc.h"
#include "clk.h"
#include "rtc.h"


void HalRtcInit(void)
{
	CLK_Osc_Setting(OSC_PORT, OSC_OSCILLATOR); /* SubOSC enable */
	RTC_Init(RTC_FIL);//使用内部低速时钟
	RTC_Start();
	date_time.year  = 0x19;
	date_time.month = 0x08;
	date_time.week	= SATURDAY; 
	date_time.day	= 0x31;
	date_time.hour	= 0x08;
	date_time.min	= 0x30;
	date_time.sec	= 0x00;
	RTC_Set_CounterValue(&date_time);
	RTC_Set_ConstPeriodInterruptOn(ONEMIN);
}

