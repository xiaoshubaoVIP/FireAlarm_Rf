#include "common.h"
#include "bsp_config.h"
#include "modules_config.h"

#include "app_radio.h"
#include "app_uart.h"

/**********************************************************************/
//Description:	PrintfVersion()										
//Parameters:                        									
//Return:   															
//Date:           
//Author:		quanwu.xu  
/**********************************************************************/
void PrintfVersion(void)
{
	debug_printf("\n\n\n\n -----------------\n");

#ifdef SUPPORT_HC32L100
	debug_printf("|      HD MCU     |\n");
#else
	debug_printf("|      ZW MCU     |\n");
#endif
	debug_printf(" -----------------\n");
#if defined(SUPPORT_X_SENSE_EN) 
	debug_printf("|*****X-SENSE*****|\n");
	debug_printf("|   EN Standard   |\n");
	debug_printf(" -----------------\n");
	debug_printf("[Hub_Freq=868.65M]\n");
	debug_printf("[Intranet_Freq=869.275M]\n");
	
#elif defined(SUPPORT_X_SENSE_UL)
	debug_printf("|*****X-SENSE*****|\n");
	debug_printf("|   UL Standard   |\n");
	debug_printf(" -----------------\n");
	debug_printf("[Hub_Freq=915.275M]\n");
	debug_printf("[Intranet_Freq=915.275M]\n");
	
#elif defined(SUPPORT_AEGISLINK_EN)
	debug_printf("|*****Agslink*****|\n");
	debug_printf("|   EN Standard   |\n");
	debug_printf(" -----------------\n");
	debug_printf("[Hub_Freq=869.675M]\n");
	debug_printf("[Intranet_Freq=869.675M]\n");

#elif defined(SUPPORT_AEGISLINK_UL)
	debug_printf("|*****Agslink*****|\n");
	debug_printf("|   UL Standard   |\n");
	debug_printf(" -----------------\n");
	debug_printf("[Hub_Freq=915.275M]\n");
	debug_printf("[Intranet_Freq=915.275M]\n");
#endif

	debug_printf("%s\n", SoftWareVer);
}



/**********************************************************************/
//Description:	EntryHaltMode()															  
//Parameters:                        												  
//Return:   																  
//Date:           
//Author:		quanwu.xu  
/**********************************************************************/
static void EntryHaltMode(void)
{
	//debug_printf("in\n");
	McuStop();
	//debug_printf("out\n");
}

/**********************************************************************/
//Description:	EntryLowpwrMode()															  
//Parameters:                        												  
//Return:   																  
//Date:           
//Author:		quanwu.xu  
/**********************************************************************/
void EntryLowpwrMode(void)
{
	if((GetSysSleepFlag() == eSysActiveFlag_NULL) && 
								(GetSystemState() == eSystemState_Idle))
	{
		EntryHaltMode();
	}
}

/**********************************************************************/
//Description:		PollingWakeupTimeout()								
//Parameters:                        									
//Return:   															
//Date:           
//Author:		quanwu.xu  
/**********************************************************************/
static void PollingWakeupTimeout(TmrHdl Handle, eTmrCmd eCmd)
{
	QEvent UartEventWakeupTimeout = {0};
	Handle = Handle;	

	switch(eCmd)
	{
		case eInsertTimer:
			break;
			
		case eTimeOut:
			TmrDeleteTimer(Handle);
			UartEventWakeupTimeout.sig = Q_eSystemEventWakeupTimeout;
			QEP_Post(&UartEventWakeupTimeout);
			break;

		case eDelTimer:
			break;

		default:
			break;
	}
}

/**********************************************************************/
//Description:	SysIdleModeHandleFunc()															  
//Parameters:                        												  
//Return:   																  
//Date:           
//Author:		quanwu.xu  
/**********************************************************************/
void SysIdleModeHandleFunc(SystemApiTag *me,QEvent *e)
{
	static TmrHdl WakeupTimeoutHeader = NULL;
	RadioDevTag* pRadioApi = GetRfApiTag();
	QEvent UartEventIdleMode = {0};

	debug_info("sig=%d\n", e->sig);

	switch (e->sig)
	{
		case Q_ENTRY_SIG:
			if(GetRfPairedMode() == ePairedHub)
			{
				Radio.RadioInit(LowPowerHubMode);
			}
			else
			{
				Radio.RadioInit(LowPowerIntranetMode);
			}

			if(UART_INT_is_H())
			{
				ResetSysSleepFlag(eSysActiveFlag_All);
			}

			me->ActiveAlarmFlag = false;
			me->SystemState = eSystemState_Idle;

			pRadioApi->RandSendState = eRandSend_Valid;
			pRadioApi->RandSendProperty = eRandSendProperty_Level_Max;
			if((me->PowerOnFlag == true) && (GetRfPairedMode() == ePairedHub))
			{
				UartEventIdleMode.sig =Q_eInitEventUpdateCancelAlarm;
				UartEventIdleMode.Value1 = FirmAlarmMsgCancelAlarm;
				QEP_Post(&UartEventIdleMode);
				me->PowerOnFlag = false;
			}
			break;
			
		case Q_EXIT_SIG:
			if(GetRfPairedMode() == ePairedHub)
			{
				Radio.RadioInit(NormalHubMode);
			}
			else
			{
				Radio.RadioInit(NormalIntranetMode);
			}
			TmrDeleteTimer(WakeupTimeoutHeader);
			ResetSysSleepFlag(eSysActiveFlag_Wakeup);
			break;
			
		case Q_eKeyEventKey1Clik:
			Q_TRAN(RFTestHandle);
			break;

		case Q_eSystemEventPairToHubMode:
			Q_TRAN(HubPairMode);
			break;

		//heartbeat
		case Q_eSystemEventUpdateHeartbeat:
			HubRfUpdateHandleFunc(eRfMsgHeartBeat, e->Value1, e->Value2, e->Value3);
			Q_TRAN(SysIdleModeHandleFunc);
			break;
		
		//uart event
		case Q_eUartEventUpdateAlarm:
			HubRfUpdateHandleFunc(eRfMsgFireAlarm, e->Value1, e->Value2, e->Value3);
			Q_TRAN(HubInitialAlarmMode);
			break;
		
		case Q_eInitEventUpdateCancelAlarm:
		case Q_eUartEventUpdateCancelAlarm:
		case Q_eUartEventUpdateCancelMute:
		case Q_eUartEventUpdateCoPpm:
		case Q_eUartEventUpdateTest:
		case Q_eUartEventUpdateBat:
		case Q_eUartEventUpdateDevFault:
			HubRfUpdateHandleFunc(eRfMsgFireAlarm, e->Value1, e->Value2, e->Value3);
			Q_TRAN(SysIdleModeHandleFunc);
			break;

		//wakeup
		case Q_eRadioEventDownloadWakeup:
			SetSysSleepFlag(eSysActiveFlag_Wakeup);
			WakeupTimeoutHeader = TmrInsertTimer(GetTmrHd(),
				TMR_PERIOD(TimeOut60s), PollingWakeupTimeout, NULL);
			break;

		case Q_eSystemEventWakeupTimeout:
			Q_TRAN(SysIdleModeHandleFunc);
			break;

		//radio function
		case Q_eRadioEventDownloadDelete:
			HubRfUpdateHandleFunc(eRfMsgDeleteDev, e->Value1, e->Value2, e->Value3);
			Q_TRAN(SysIdleModeHandleFunc);
			break;

		case Q_eRadioEventDownloadTest:
			UartDownLoadHandleFunc(eUartMsgFireAlarm, e->Value1, e->Value2);
			Q_TRAN(SysIdleModeHandleFunc);
			break;

		case Q_eRadioEventDownloadAlarm:
			UartDownLoadHandleFunc(eUartMsgFireAlarm, e->Value1, e->Value2);
			Q_TRAN(HubAlarmMode);
			break;
			

#ifdef SUPPORT_INTRANET_FUNCTION
		case Q_eIntranetEventPairFactoryMode:
			Q_TRAN(IntranetPairFactoryModeEvent);
			break;
		
		case Q_eIntranetEventPairIntranetMasterMode:
			Q_TRAN(IntranetPairMasterModeEvent);
			break;
	
		case Q_eIntranetEventPairIntranetSlaverMode:
			Q_TRAN(IntranetPairSlaverModeEvent);
			break;
	
		case Q_eIntranetEventBroadcastTest:
			Q_TRAN(IntranetBroadcastTestModeEvent);
			break;
	
		case Q_eIntranetEventMasterTest:
		case Q_eIntranetEventForwardTest:
			Q_TRAN(&IntranetForwardTestModeEvent);
			break;
	
		case Q_eIntranetEventBroadcastSmokeAlarm:
			Q_TRAN(&IntranetBroadcastSmokeAlarmModeEvent);
			break;
		
		case Q_eIntranetEventBroadcastCoAlarm:
			Q_TRAN(&IntranetBroadcastCoAlarmModeEvent);
			break;
	
		case Q_eIntranetEventSmokeAlarm:
			Q_TRAN(&IntranetForwardSmokeAlarmModeEvent);
			break;
		
		case Q_eIntranetEventCoAlarm:
			Q_TRAN(&IntranetForwardCoAlarmModeEvent);
			break;
#endif
		default:
			break;
	}
}

/**********************************************************************/
//Description:	SysStartHandleFunc()												
//Parameters:                        										
//Return:   																
//Date:           
//Author:		quanwu.xu  
/**********************************************************************/
void SysStartHandleFunc(SystemApiTag *me,QEvent *e)
{
	me->PowerOnFlag = true;
	me->ActiveAlarmFlag = false;
	me->SystemState = eSystemState_Idle;
	me->SystemTick = 0;
	me->SysActiveFlag = eSysActiveFlag_NULL;

	PollingModuleInit();
	KeyInit();

	UartApiTagInit();
	RadioApiTagInit();
	
	Q_INIT(&SysIdleModeHandleFunc);
}


/**********************************************************************/
//Description:	ApplicationInit()												
//Parameters:                        										
//Return:   																
//Date:           
//Author:		quanwu.xu  
/**********************************************************************/
void ApplicationInit(void)
{
	PrintfVersion();
	QEP_Init((QFsm *)&SystemApiTagObj, (QState)&SysStartHandleFunc);
}


