#include "common.h"

#include "bsp_config.h"
#include "modules_config.h"

#include "app_uart.h"
#include "app_radio.h"
#include "app_main.h"


const uint8_t UniversalID[RfIdLength] = {0xFF, 0xFF, 0xFF, 0xFE};

#define MIX_RAND_NUM			1			//100ms
#define MAX_RAND_NUM 			49			//2000ms
#define INTERVAL_DEDAY_TIME		100			//ms

#define BoradcastDevice		((uint8_t)1)
#define ForwardDevice		((uint8_t)2)

static RfSendMsgParamer GlobalSendMsgParamer ={0};

typedef enum _eRadioDownState
{
	RadioDownState_Idle = 0x00,
	RadioDownState_Start,
	RadioDownState_Timeout,
	RadioDownState_Success,
	RadioDownState_Fail,
	RadioDownState_DeleteMsgStop,
}eRadioDownState;

typedef struct _RadioSendTag
{
	bool ReSendFlag;
	eRadioDownState RadioDownState;
	TmrHdl TimerSendRfMsgHandle;
}RadioSendTag;

static RadioSendTag RadioSendTagObject = {true, RadioDownState_Idle, NULL};

RadioDevTag RadioDevTagObj;
RadioDevTag* pRadioDevTagObj = &RadioDevTagObj;

void PollingSendMsgTimeout(TmrHdl Handle, eTmrCmd eCmd);
#ifdef SUPPORT_INTRANET_FUNCTION
void IntranetAlarmModeEvent(SystemApiTag  *me,QEvent *e);
void IntranetBroadcastCancelTestModeEvent(SystemApiTag  *me,QEvent *e);
void IntranetForwardCancelTestModeEvent(SystemApiTag  *me,QEvent *e);
void IntranetBroadcastCancelAlarmModeEvent(SystemApiTag  *me,QEvent *e);
void IntranetForwardCancelAlarmModeEvent(SystemApiTag  *me,QEvent *e);
#endif

#define RadioQueueLimit		((uint8_t)64)
#define RadioBufLimit		((uint8_t)96)

unsigned char volatile RadioQueueBuf[RadioQueueLimit] = {0};	//队列缓存
unsigned char RadioReadBuf[RadioBufLimit] = {0};				//接收缓存

volatile unsigned char *rf_queue_in = (unsigned char *)RadioQueueBuf;
volatile unsigned char *rf_queue_out= (unsigned char *)RadioQueueBuf;

bool RadioRxIntflag = false;

/**********************************************************************/
//Description:	SetRadioRxIntFlag()													  
//Parameters:       
//                        												  
//Return:   
//Date			2019-01-17
//Author:		quanwu.xu
/**********************************************************************/
void SetRadioRxIntFlag(void)
{
	RadioRxIntflag = true;
}

/**********************************************************************/
//Description:	ClrRadioRxIntFlag()													  
//Parameters:       
//                        												  
//Return:   
//Date			2019-01-17
//Author:		quanwu.xu
/**********************************************************************/
void ClrRadioRxIntFlag(void)
{
	RadioRxIntflag = false;
}

/*****************************************************************************
函数名称 : get_queue_total_data
功能描述 : 读取队列内数据
输入参数 : 无
返回参数 : 无
*****************************************************************************/
static unsigned char Rf_get_queue_total_data(void)
{
	if(rf_queue_in != rf_queue_out)
		return 1;
	else
		return 0;
}
/*****************************************************************************
函数名称 : Queue_Read_Byte
功能描述 : 读取队列1字节数据
输入参数 : 无
返回参数 : 无
*****************************************************************************/
static unsigned char Rf_Queue_Read_Byte(void)
{
	unsigned char value = 0;

	if(rf_queue_out != rf_queue_in)
	{
		//有数据
		if(rf_queue_out >= (unsigned char *)(RadioQueueBuf + sizeof(RadioQueueBuf)))
		{
			//数据已经到末尾
			rf_queue_out = (unsigned char *)(RadioQueueBuf);
		}

		value = *rf_queue_out++;
	}

	return value;
}


/*****************************************************************************
函数名称 : uart_receive_input
功能描述 : 收数据处理
输入参数 : value:串口收到字节数据
返回参数 : 无
使用说明 : 在MCU串口接收函数中调用该函数,并将接收到的数据作为参数传入
*****************************************************************************/
void RadioReceiveInput(unsigned char value)
{
	if((rf_queue_in > rf_queue_out) && ((rf_queue_in - rf_queue_out) >= sizeof(RadioQueueBuf)))
	{
		//数据队列满
	}
	else if((rf_queue_in < rf_queue_out) && ((rf_queue_out  - rf_queue_in) == 1))
	{
		//数据队列满
	}
	else
	{
		//队列不满
		if(rf_queue_in >= (unsigned char *)(RadioQueueBuf + sizeof(RadioQueueBuf)))
		{
			rf_queue_in = (unsigned char *)(RadioQueueBuf);
		}

		*rf_queue_in++ = value;
	}
}

/**********************************************************************/
//Description:	GetRfPairedMode()											
//Parameters:                        									
//Return:   																  
//Date:           
//Author:		quanwu.xu  
/**********************************************************************/
ePairedMode GetRfPairedMode(void)
{
	return pRadioDevTagObj->PairMode;
}


/**********************************************************************/
//Description:	GetRfApiTag()											
//Parameters:                        									
//Return:   																  
//Date:           
//Author:		quanwu.xu  
/**********************************************************************/
RadioDevTag* GetRfApiTag(void)
{
	return pRadioDevTagObj;
}

/**********************************************************************/
//Description:	SaveFlashInfo()															  
//Parameters:                        												  
//Return:   																  
//Date:           
//Author:		quanwu.xu  
/**********************************************************************/
static bool SaveFlashInfo(uint32_t FashAddr, uint32_t Data)
{
	bool enResult = false;
	uint32_t ReadData = 0xFF;

	hal_flash_write(FashAddr, Data);
	hal_flash_read(&ReadData, FashAddr);
	if (Data == ReadData)
	{
		enResult = true;
	}
	return enResult;
}

/**********************************************************************/
//Description:	CompareRfID															  
//Parameters:                        												  
//Return:   																  
//Date:           
//Author:		quanwu.xu  
/**********************************************************************/
bool CompareRfID(const uint8_t *src, const uint8_t *dest, uint8_t Length)
{
	uint8_t i = 0;
	bool Ret = true;
	
	for(i=0;i<Length; i++)
	{
		if(src[i] != dest[i])
		{
			Ret = false;	
			break;
		}
	}
	return Ret;
}

/**********************************************************************/
//Description:	SaveIDProcess()											
//Parameters:   Device Rx a New ID, the give an Ack Msg with the Device Num 
//Return:   															
//Date:           
//Author:		quanwu.xu  
/**********************************************************************/
bool SaveIDProcess(uint8_t *pID, bool SaveIdFlag, uint8_t SavePairMode)
{
	uint8_t i = 0;
	uint32_t SaveData = 0;

	RadioDevTag* pRadioApi = GetRfApiTag();


	hal_flash_init();

	SaveData = 0;
	for(i=0; i<RfIdLength; i++)
	{
		SaveData += (uint32_t)(pRadioApi->RadioUdid[i]<<(i<<3));
	}

	if(SaveFlashInfo(FLASH_ADDR_UDID_NUM, SaveData) == false)
	{
		return false;
	}

	if(SavePairMode == ePairedResetId)											//save UDID as RF ID
	{
		if((SaveFlashInfo(FLASH_ADDR_RF_ID, SaveData) == false) ||
			(SaveFlashInfo(FLASH_ADDR_PAIR_FLAG, (uint32_t)SavePairMode) == false))
		{
			return false;
		}
	}

	if(SaveIdFlag == false)
	{
		return true;
	}

	SaveData = 0;
	for(i=0; i<RfIdLength; i++)
	{
		SaveData += (uint32_t)(pID[i]<<(i<<3));
	}
	
	if((SaveFlashInfo(FLASH_ADDR_RF_ID, SaveData) == true) && 
		(SaveFlashInfo(FLASH_ADDR_DEV_NUM, (uint32_t)pRadioApi->SelfDevNum) == true) &&
			(SaveFlashInfo(FLASH_ADDR_DEV_TYPE, (uint32_t)pRadioApi->DevType) == true) &&
				(SaveFlashInfo(FLASH_ADDR_PRO_TYPE, (uint32_t)pRadioApi->ProductType) == true) &&
					(SaveFlashInfo(FLASH_ADDR_FATHER_NUM, (uint32_t)pRadioApi->FatherDevNum) == true) &&
						(SaveFlashInfo(FLASH_ADDR_PAIR_FLAG, (uint32_t)SavePairMode) == true))
	{
		memcpy((void *)pRadioApi->RadioDevId, (void*)pID, RfIdLength);
		
		debug_printf("Save ID: %x %x %x %x\n", pRadioApi->RadioDevId[0], pRadioApi->RadioDevId[1],
										pRadioApi->RadioDevId[2], pRadioApi->RadioDevId[3]);
		debug_printf("Father_Num.%d, Father_DevType=%d\n", pRadioApi->FatherDevNum, pRadioApi->FatherDevType);

		HalDelayMs(5);		
		return true;
	}
	else
	{
		debug_warning("Save ID fail\n");
		return false;
	}
}


/**********************************************************************/
//Description:	RadioApiTagInit()										
//Parameters:                        									
//Return:   															
//Date:           
//Author:		quanwu.xu  
/**********************************************************************/
void RadioApiTagInit(void)
{
	uint8_t i = 0;
	uint32_t ReadData = 0;	

	RadioDevTag* pRadioApi = GetRfApiTag();

#ifdef SUPPORT_TYPE_HUB
	pRadioApi->DevType = eTypeHub;
#elif defined(SUPPORT_TYPE_REPEATER)
	pRadioApi->DevType = eTypeRepeater;
	pRadioApi->ProductType = eProductType_Repeater1_Smart;
#else
	pRadioApi->DevType = eTypeNull;
	pRadioApi->ProductType = eProductType_NULL;
#endif
	pRadioApi->SelfDevNum = TxToAllFireAlarms;

	pRadioApi->FatherDevType = eTypeNull;
	pRadioApi->FatherDevNum = DEFAULT_DEV_NUM;
	
	pRadioApi->RandSendState = eRandSend_Valid;
	pRadioApi->RandSendProperty = eRandSendProperty_Level_Max;
	pRadioApi->RxRandSendProperty = eRandSendProperty_Level_Max;

	memset(pRadioApi->RadioDevId, 0, RfIdLength);

	hal_flash_read(&ReadData, FLASH_ADDR_UDID_NUM);
	memcpy((void *)pRadioApi->RadioUdid, (void *)&ReadData, RfIdLength);

	debug_printf("UDID: 0x");
	for(i=0; i<RfIdLength; i++)
	{
		debug_printf("%02x", pRadioApi->RadioUdid[i]);
	}
	debug_printf(" \n");
	
	hal_flash_read(&ReadData, FLASH_ADDR_PAIR_FLAG);
	pRadioApi->PairMode = (ePairedMode)ReadData;
	if(pRadioApi->PairMode == ePairedHub)
	{
		HalRtcInit();
	
		hal_flash_read(&ReadData, FLASH_ADDR_RF_ID);	
		memcpy((void *)pRadioApi->RadioDevId, (void *)&ReadData, RfIdLength);

		debug_printf("Gateway ID: 0x");
		for(i=0; i<RfIdLength; i++)
		{
			debug_printf("%02x", pRadioApi->RadioDevId[i]);
		}
		debug_printf(" \n");

		hal_flash_read(&ReadData, FLASH_ADDR_FATHER_NUM);
		if(ReadData != 0xFFFFFFFF)
		{
			pRadioApi->FatherDevNum = ReadData;
			if(pRadioApi->FatherDevNum == HubNumber)
			{
				pRadioApi->FatherDevType = eTypeHub;
			}
			else
			{
				pRadioApi->FatherDevType = eTypeRepeater;
			}
		}
		
		hal_flash_read(&ReadData, FLASH_ADDR_DEV_NUM);
		if(ReadData != 0xFFFFFFFF)
		{
			pRadioApi->SelfDevNum = ReadData;
		}

		hal_flash_read(&ReadData, FLASH_ADDR_DEV_TYPE);
		if(ReadData != 0xFFFFFFFF)
		{
			pRadioApi->DevType = (eRfDeviceType)ReadData;
		}

		hal_flash_read(&ReadData, FLASH_ADDR_PRO_TYPE);
		if(ReadData != 0xFFFFFFFF)
		{
			pRadioApi->ProductType = (eProductType)ReadData;
		}
		
		debug_printf("Num.%d, DevType=%d, ProType=%d ", pRadioApi->SelfDevNum, pRadioApi->DevType, pRadioApi->ProductType);
		debug_printf("Father_Num.%d, Father_DevType=%d\n", pRadioApi->FatherDevNum, pRadioApi->FatherDevType);
	}
	else if(pRadioApi->PairMode == ePairedIntranet)
	{
		hal_flash_read(&ReadData, FLASH_ADDR_RF_ID);	
		memcpy((void *)pRadioApi->RadioDevId, (void *)&ReadData, RfIdLength);

		debug_printf("Intranet ID: 0x");
		for(i=0; i<RfIdLength; i++)
		{
			debug_printf("%02x", pRadioApi->RadioDevId[i]);
		}
		debug_printf(" \n");
	}
	else if(pRadioApi->PairMode == ePairedResetId)
	{
		hal_flash_read(&ReadData, FLASH_ADDR_RF_ID);	
		memcpy((void *)pRadioApi->RadioDevId, (void *)&ReadData, RfIdLength);

		debug_printf("Reset ID: 0x");
		for(i=0; i<RfIdLength; i++)
		{
			debug_printf("%02x", pRadioApi->RadioDevId[i]);
		}
		debug_printf(" \n");
	}
	else
	{
		pRadioApi->PairMode = ePairedNull;
		debug_warning("not paired\n");
	}

	HalGpioSetMode(PIN_RF_INT, PIN_MODE_INPUT_PULLDOWN);
	HalGpioSetIRQMode(PIN_RF_INT, PIN_IRQ_RISING);

#if 0
	for(i=0; i<3; i++)
	{
		UartService();
		HalDelayMs(1000);
	}
#endif

	Radio.RadioInit(LowPowerHubMode);
}

/**********************************************************************/
//Description:		SetPlayloadBufDataPart1()									
//Parameters:															
//Return:																
//Date: 		  
//Author:			quanwu.xu  
/**********************************************************************/
void SetPlayloadBufDataPart1(RfSendMsgParamer *pSendMsgParamer, uint8_t MsgTargetNum,
	eHubMsgType MsgLevel1Type, uint8_t MsgLevel2Type, uint8_t Property, uint8_t PackNum)
{
	pSendMsgParamer->MsgTargetNum = MsgTargetNum;
	pSendMsgParamer->MsgLevel1Type = MsgLevel1Type;
	pSendMsgParamer->MsgLevel2Type = MsgLevel2Type;
	pSendMsgParamer->Property = Property;
	pSendMsgParamer->PackNum = PackNum;
}

/**********************************************************************/
//Description:		SetPlayloadBufDataPart2()							
//Parameters:															
//Return:																
//Date: 		  
//Author:			quanwu.xu  
/**********************************************************************/
void SetPlayloadBufDataPart2(RfSendMsgParamer *pSendMsgParamer, uint8_t Data1, 
	uint8_t Data2, uint8_t Data3, uint8_t Data4, eRadioInitMode InitMode)
{
	pSendMsgParamer->Data1 = Data1;
	pSendMsgParamer->Data2 = Data2;
	pSendMsgParamer->Data3 = Data3;
	pSendMsgParamer->Data4 = Data4;
	pSendMsgParamer->Mode = InitMode;
}


/**********************************************************************/
//Description:		HubSend()															  
//Parameters:                        												  
//Return:   																  
//Date:           
//Author:			quanwu.xu  
/**********************************************************************/
void HubSend(RadioMsgTag *pRadioMsg, eRadioInitMode InitMode, uint16_t PackNum)
{
	uint8_t i = 0;	

	debug_printf("Tx:");
	for(i=0; i<Radio.PlayloadLen; i++)
	{
		debug_printf("%x ", ((uint8_t *)pRadioMsg)[i]);
	}
	debug_printf("\n");

	Radio.RadioInit(InitMode);
	Radio.RadioSend((uint8_t *)pRadioMsg, Radio.PlayloadLen, PackNum, 6000);
}

/**********************************************************************/
//Description:		HubSendAckMsg()															  
//Parameters:                        												  
//Return:   																  
//Date:           
//Author:			quanwu.xu  
/**********************************************************************/
void HubSendAckMsg(uint8_t TargetNum, eHubMsgType Level1Type, uint8_t Ack, uint8_t Level2Type)
{
	RadioDevTag* pRadioApi = GetRfApiTag();
	RadioMsgTag RfMsgTag = {0};
	
	memcpy((void*)RfMsgTag.DeviceId, (void *)pRadioApi->RadioDevId, RfIdLength);
	RfMsgTag.DeviceNumber = pRadioApi->SelfDevNum;
	RfMsgTag.TargetNumber = TargetNum;
	RfMsgTag.DeviceType = pRadioApi->DevType;
	RfMsgTag.MsgLevel1 = Level1Type;
	RfMsgTag.MsgLevel2 = Ack;
	RfMsgTag.MsgData1 = Level2Type;
	RfMsgTag.MsgProperty = RfMsgPropertyNull;
	RfMsgTag.MsgCrc = GetCrc8((void*)&RfMsgTag, Radio.PlayloadLen - 1);

	HubSend(&RfMsgTag, NormalHubMode, PackNum_1);
}


/**********************************************************************/
//Description:		SendRadioMsg()									
//Parameters:                        									
//Return:   															
//Date:           
//Author:			quanwu.xu  
/**********************************************************************/
void HubSendRadioMsg(RfSendMsgParamer *pSendMsgParamer, bool ReTransmitFlag)
{
	RadioDevTag* pRadioApi = GetRfApiTag();
	RadioMsgTag RfMsgTag = {0};
	uint32_t RandTime = 0;
	eDelayMode DelayMode = eNullTimeMode;
		
	memcpy((void*)RfMsgTag.DeviceId, (void *)pRadioApi->RadioDevId, RfIdLength);
	RfMsgTag.DeviceNumber = pRadioApi->SelfDevNum;
	RfMsgTag.TargetNumber = pSendMsgParamer->MsgTargetNum;
	RfMsgTag.DeviceType = pRadioApi->DevType;
	RfMsgTag.MsgLevel1 = pSendMsgParamer->MsgLevel1Type;
	RfMsgTag.MsgLevel2 = pSendMsgParamer->MsgLevel2Type;
	RfMsgTag.MsgProperty = pSendMsgParamer->Property;

	RfMsgTag.MsgData1 = pSendMsgParamer->Data1;
	RfMsgTag.MsgData2 = pSendMsgParamer->Data2;	
	RfMsgTag.MsgData3 = pSendMsgParamer->Data3;
	RfMsgTag.MsgData4 = pSendMsgParamer->Data4;
	RfMsgTag.MsgCrc = GetCrc8((void*)&RfMsgTag, Radio.PlayloadLen - 1);

	memcpy(&GlobalSendMsgParamer, pSendMsgParamer, sizeof(RfSendMsgParamer));
	
	HubSend(&RfMsgTag, pSendMsgParamer->Mode, pSendMsgParamer->PackNum);
	
	Radio.WaitSendDone();

	if(pSendMsgParamer->Mode == LowPowerHubMode)
	{
		DelayMode = eLowpowerTimeMode;
	}
	else if(pSendMsgParamer->Mode == NormalHubMode)
	{
		DelayMode = eNormalTimeMode;
	}

	if(ReTransmitFlag == true)
	{
		if(RfMsgTag.MsgProperty & RfMsgPropertyNeedAck)
		{
			if(RadioSendTagObject.TimerSendRfMsgHandle == NULL)
			{
				RandTime = GetRandDelayTime(DelayMode);
				RadioSendTagObject.TimerSendRfMsgHandle = TmrInsertTimer(GetTmrHd(),TMR_PERIOD(RandTime), 
																			PollingSendMsgTimeout, NULL);
			}
		}
	}
}

/**********************************************************************/
//Description:		PollingSendMsgTimeout()															  
//Parameters:                        												  
//Return:   																  
//Date:           
//Author:		quanwu.xu  
/**********************************************************************/
void PollingSendMsgTimeout(TmrHdl Handle, eTmrCmd eCmd)
{
	static uint8_t ReTxCnt = 0;

	Handle = Handle;

	switch(eCmd)
	{
		case eInsertTimer:
			RadioSendTagObject.RadioDownState = RadioDownState_Start;
			ReTxCnt = 0;
			break;
			
		case eTimeOut:
			if(ReTxCnt++>2)
			{
				TmrDeleteTimer(RadioSendTagObject.TimerSendRfMsgHandle);
				RadioSendTagObject.TimerSendRfMsgHandle = NULL;
				RadioSendTagObject.RadioDownState = RadioDownState_Fail;
			}
			else
			{
				HubSendRadioMsg(&GlobalSendMsgParamer, true);
			}
			break;

		case eDelTimer:
			break;

		default:
			break;
	}	
}

/**********************************************************************/
//Description:		PollingPairTimeout()															  
//Parameters:                        												  
//Return:   																  
//Date:           
//Author:		quanwu.xu  
/**********************************************************************/
static void PollingPairTimeout(TmrHdl Handle, eTmrCmd eCmd)
{
	QEvent RadioEventPairTimeout = {0};

	Handle = Handle;
	switch(eCmd)
	{
		case eTimeOut:
			RadioEventPairTimeout.sig = Q_eSystemEventPairFail;
			QEP_Post(&RadioEventPairTimeout);
			break;

		default:
			break;
	}	
}

/**********************************************************************/
//Description:	PollingPairRequest()															  
//Parameters:                        												  
//Return:   																  
//Date:           quanwu.xu  
/**********************************************************************/
void PollingPairRequest(TmrHdl Handle, eTmrCmd eCmd)
{
	RfSendMsgParamer SendMsgParamer = {0};
	RadioDevTag* pRadioApi = GetRfApiTag();

	Handle = Handle;

	SetPlayloadBufDataPart1(&SendMsgParamer, HubNumber, eRfMsgPair, PairMsgRequest, 
													RfMsgPropertyNeedAck, PackNum_1);
	SetPlayloadBufDataPart2(&SendMsgParamer, pRadioApi->RadioUdid[0], pRadioApi->RadioUdid[1],
								pRadioApi->RadioUdid[2], pRadioApi->RadioUdid[3], NormalHubMode);
	switch(eCmd)
	{
		case eInsertTimer:
		case eTimeOut:
			HubSendRadioMsg(&SendMsgParamer, false);
			break;

		default:
			break;
	}
}

/**********************************************************************/
//Description:	PollingEntryIdleMode()															  
//Parameters:                        												  
//Return:   																  
//Date:           quanwu.xu  
/**********************************************************************/
void PollingEntryIdleMode(TmrHdl Handle, eTmrCmd eCmd)
{
	QEvent RadioEventEntryLowpower = {0};

	Handle = Handle;
	switch(eCmd)
	{
		case eInsertTimer:
			break;
	
		case eTimeOut:
			RadioEventEntryLowpower.sig =Q_eSystemEventLowpowerMode;
			QEP_Post(&RadioEventEntryLowpower);
			break;

		case eDelTimer:
			break;

		default:
			break;
	}
} 


/**********************************************************************/
//Description:	HubRfWakeupDevMsgHandle()															  
//Parameters:                        												  
//Return:   																  
//Date:           
//Author:		quanwu.xu  
/**********************************************************************/
void HubRfWakeupDevMsgHandle(RadioMsgTag *pRadioMsg)
{
	uint8_t RadioMsgLeve2 = pRadioMsg->MsgLevel2;
	RadioDevTag* pRadioApi = GetRfApiTag();
	QEvent RadioEventWakeup = {0};
	
	switch (RadioMsgLeve2)
	{
		case WakeupMsgAck:
			{
				TmrDeleteTimer(RadioSendTagObject.TimerSendRfMsgHandle);
				RadioSendTagObject.TimerSendRfMsgHandle = NULL;
				RadioSendTagObject.RadioDownState = RadioDownState_Success;
			}
			break;
		
		case WakeupMsgAllDev:
			if((pRadioMsg->TargetNumber == TxToAllFireAlarms) && 
				((pRadioApi->DevType == eTypeSmoke) || (pRadioApi->DevType == eTypeCo) ||
				(pRadioApi->DevType == eTypeSmokeAndCo)))
			{
				Radio.RadioInit(NormalHubMode);
				RadioEventWakeup.sig =Q_eRadioEventDownloadWakeup;
				QEP_Post(&RadioEventWakeup);
			}
			break;
			
		case WakeupMsgSingleDev:
			break;
			
		default:
			break;
	}	
}



/**********************************************************************/
//Description:	HubRfDeleteDevMsgHandle()															  
//Parameters:                        												  
//Return:   																  
//Date:           
//Author:		quanwu.xu  
/**********************************************************************/
void HubRfDeleteDevMsgHandle(RadioMsgTag *pRadioMsg)
{	
	uint8_t RadioMsgLeve2 = pRadioMsg->MsgLevel2;
	RadioDevTag* pRadioApi = GetRfApiTag();
	QEvent RadioEventDelete = {0};

	if(RadioMsgLeve2 != DeleteDevMsgAck)
	{
		HubSendAckMsg(pRadioMsg->DeviceNumber, eRfMsgDeleteDev, DeleteDevMsgAck, RadioMsgLeve2);
	}

	switch(RadioMsgLeve2)
	{
		case DeleteDevMsgAck:
		{
			if(GlobalSendMsgParamer.MsgLevel2Type == pRadioMsg->MsgData1)
			{
				TmrDeleteTimer(RadioSendTagObject.TimerSendRfMsgHandle);
				RadioSendTagObject.TimerSendRfMsgHandle = NULL;
				RadioSendTagObject.RadioDownState = RadioDownState_Success;
				if(SaveIDProcess(pRadioApi->RadioDevId, false, ePairedHub))
				{
					SoftwareReset();
				}
			}
		}
		break;

		case DeleteDevMsgNotice:
			RadioSendTagObject.RadioDownState = RadioDownState_DeleteMsgStop;
			RadioEventDelete.sig =Q_eRadioEventDownloadDelete;
			RadioEventDelete.Value1 = DeleteDevMsgSuccess;
			QEP_Post(&RadioEventDelete);
			break;

		default:
			break;
	}
}


/**********************************************************************/
//Description:	HubRfSearchDevMsgHandle()															  
//Parameters:                        												  
//Return:   																  
//Date:           
//Author:		quanwu.xu  
/**********************************************************************/
void HubRfSearchDevMsgHandle(RadioMsgTag *pRadioMsg)
{
	uint8_t RadioMsgLeve2 = pRadioMsg->MsgLevel2;
	RadioDevTag* pRadioApi = GetRfApiTag();
	QEvent RadioEventSearch = {0};

	if((pRadioMsg->DeviceType == eTypeHub) || (pRadioMsg->DeviceType == eTypeRepeater))
	{
		if(RadioMsgLeve2 != SearchMsgAck)
		{
			HubSendAckMsg(pRadioMsg->DeviceNumber, eRfMsgSearchDev, SearchMsgAck, RadioMsgLeve2);
		}

		switch (RadioMsgLeve2)
		{
			case SearchMsgAck:
				{
					if(GlobalSendMsgParamer.MsgLevel2Type == pRadioMsg->MsgData1)
					{
						TmrDeleteTimer(RadioSendTagObject.TimerSendRfMsgHandle);
						RadioSendTagObject.TimerSendRfMsgHandle = NULL;
						RadioSendTagObject.RadioDownState = RadioDownState_Success;
					}
				}
				break;

			case SearchMsgSuccess:
				if(pRadioMsg->DeviceNumber == HubNumber)
				{
					pRadioApi->FatherDevNum = HubNumber;
					pRadioApi->FatherDevType = eTypeHub;
					
					if(SaveIDProcess(pRadioMsg->DeviceId, true, ePairedHub))
					{					
						RadioEventSearch.sig = Q_eRadioSearchHubNodeSuccess;			//Search hub ok
						QEP_Post(&RadioEventSearch);
					}
				}
				else
				{
					pRadioApi->FatherDevNum = pRadioMsg->DeviceNumber;
					pRadioApi->FatherDevType = eTypeRepeater;
					
					if(SaveIDProcess(pRadioMsg->DeviceId, true, ePairedHub))
					{					
						RadioEventSearch.sig = Q_eRadioSearchRepeaterNodeSuccess;		//Search repeater ok
						QEP_Post(&RadioEventSearch);
					}
				}
				break;

			case SearchMsgFail:
				RadioEventSearch.sig = Q_eRadioSearchNodeFail;
				QEP_Post(&RadioEventSearch);
				break;

			default:
				break;
		}
	}
}

/**********************************************************************/
//Description:	HubRfHeartbeatMsgHandle()															  
//Parameters:                        												  
//Return:   																  
//Date:           
//Author:		quanwu.xu  
/**********************************************************************/
void HubRfHeartbeatMsgHandle(RadioMsgTag *pRadioMsg)
{
	uint8_t RadioMsgLeve2 = pRadioMsg->MsgLevel2;
	RadioDevTag* pRadioApi = GetRfApiTag();

	if(RadioMsgLeve2 != HearbeatMsgAck)
	{
		HubSendAckMsg(pRadioMsg->DeviceNumber, eRfMsgHeartBeat, HearbeatMsgAck, RadioMsgLeve2);
	}
	
	switch (RadioMsgLeve2)
	{
		case HearbeatMsgAck:
			{
				if(HearbeatMsgUpdate == pRadioMsg->MsgData1)
				{
					TmrDeleteTimer(RadioSendTagObject.TimerSendRfMsgHandle);
					RadioSendTagObject.TimerSendRfMsgHandle = NULL;
					RadioSendTagObject.RadioDownState = RadioDownState_Success;
					RadioSendTagObject.ReSendFlag = true;
				}
			}
			break;
			
		case HearbeatMsgUpdate:
			break;

		default:
			break;

	}
}

/**********************************************************************/
//Description:	HubRfFireAlarmMsgHandle()															  
//Parameters:                        												  
//Return:   																  
//Date:           
//Author:		quanwu.xu  
/**********************************************************************/
void HubRfFireAlarmMsgHandle(RadioMsgTag *pRadioMsg)
{
	uint8_t RadioMsgLeve2 = pRadioMsg->MsgLevel2;
	QEvent RadioEventFireAlarm = {0};
	RadioDevTag* pRadioApi = GetRfApiTag();

	if((pRadioMsg->MsgData1 == DevTestStart) && (RadioMsgLeve2 == FirmAlarmMsgTest))
	{
		Radio.RadioInit(NormalHubMode);
		HalDelayMs(300);
	}
	
	if((RadioMsgLeve2 != FirmAlarmMsgAck) && (pRadioMsg->MsgProperty & RfMsgPropertyNeedAck))
	{
		HubSendAckMsg(pRadioMsg->DeviceNumber, eRfMsgFireAlarm, FirmAlarmMsgAck, RadioMsgLeve2);
	}

	switch (RadioMsgLeve2)
	{
		case FirmAlarmMsgAck:
			{
				if(GlobalSendMsgParamer.MsgLevel2Type == pRadioMsg->MsgData1)
				{
					TmrDeleteTimer(RadioSendTagObject.TimerSendRfMsgHandle);
					RadioSendTagObject.TimerSendRfMsgHandle = NULL;
					RadioSendTagObject.RadioDownState = RadioDownState_Success;
				}
			}
			break;

		case FirmAlarmMsgTest:
			if(pRadioApi->PairMode == ePairedHub)
			{
				RadioEventFireAlarm.sig = Q_eRadioEventDownloadTest;
				RadioEventFireAlarm.Value1 = UartFireAlarmMsgTest;
				RadioEventFireAlarm.Value2 = DevTestStart;
			}
			QEP_Post(&RadioEventFireAlarm);
			break;

		case FirmAlarmMsgMute:
			RadioEventFireAlarm.sig =Q_eRadioEventDownloadMute;
			QEP_Post(&RadioEventFireAlarm);
			break;
			
		case FirmAlarmMsgCancelMute:
			RadioEventFireAlarm.sig =Q_eRadioEventDownloadCancelMute;
			QEP_Post(&RadioEventFireAlarm);
			break;

		case FirmAlarmMsgSmokeAlarm:
			if(pRadioApi->PairMode == ePairedHub)
			{
				RadioEventFireAlarm.sig = Q_eRadioEventDownloadAlarm;
				RadioEventFireAlarm.Value1 = UartFireAlarmMsgSmokeAlarm;
				QEP_Post(&RadioEventFireAlarm);
			}
			break;

		case FirmAlarmMsgCoAlarm:
			if(pRadioApi->PairMode == ePairedHub)
			{
				RadioEventFireAlarm.sig = Q_eRadioEventDownloadAlarm;
				RadioEventFireAlarm.Value1 = UartFireAlarmMsgCoAlarm;
				QEP_Post(&RadioEventFireAlarm);
			}
			break;
			
		case FirmAlarmMsgHeatAlarm:
			if(pRadioApi->PairMode == ePairedHub)
			{
				RadioEventFireAlarm.sig = Q_eRadioEventDownloadAlarm;
				RadioEventFireAlarm.Value1 = UartFireAlarmMsgHeatAlarm;
				QEP_Post(&RadioEventFireAlarm);
			}
			break;
			
		case FirmAlarmMsgGasAlarm:
			if(pRadioApi->PairMode == ePairedHub)
			{
				RadioEventFireAlarm.sig = Q_eRadioEventDownloadAlarm;
				RadioEventFireAlarm.Value1 = UartFireAlarmMsgGasAlarm;
				QEP_Post(&RadioEventFireAlarm);
			}
			break;

		case FirmAlarmMsgCancelAlarm:
			if(pRadioApi->PairMode == ePairedHub)
			{
				RadioEventFireAlarm.sig = Q_eRadioEventDownloadCancelAlarm;
				RadioEventFireAlarm.Value1 = UartFireAlarmMsgCancelAlarm;
				QEP_Post(&RadioEventFireAlarm);
			}
			break;

		default:
			break;
	}
}


/**********************************************************************/
//Description:	HubRfPairMsgHandle()										
//Parameters:                        									
//Return:   															
//Date:           
//Author:		quanwu.xu  
/**********************************************************************/
void HubRfPairMsgHandle(RadioMsgTag *pRadioMsg, eSystemState States)
{
	uint8_t DevUdid[RfIdLength] = {0};
	RadioDevTag* pRadioApi = GetRfApiTag();
	uint8_t RadioMsgLeve2 = pRadioMsg->MsgLevel2;
	QEvent RadioEventPair = {0};

	if((pRadioMsg->DeviceId[0] == 0) && (pRadioMsg->DeviceId[1] == 0) &&
					(pRadioMsg->DeviceId[2] == 0) && (pRadioMsg->DeviceId[3] == 0))
	{
		debug_printf("Hub ID is 0\n");
	}
	else
	{
		memcpy(DevUdid, &pRadioMsg->MsgData1, RfIdLength);
		switch (RadioMsgLeve2)
		{
			case PairMsgAck:
				if(GlobalSendMsgParamer.MsgLevel2Type == pRadioMsg->MsgData1)
				{
					TmrDeleteTimer(RadioSendTagObject.TimerSendRfMsgHandle);
					RadioSendTagObject.TimerSendRfMsgHandle = NULL;
					RadioSendTagObject.RadioDownState = RadioDownState_Success;
					if(PairMsgSuccess == pRadioMsg->MsgData1)
					{
						if(States == eSystemState_Pair)
						{
							if(SaveIDProcess(pRadioMsg->DeviceId, true, ePairedHub))
							{
								SoftwareReset();
							}
						}
					}
				}
				break;

			case PairMsgStart:
				if(States == eSystemState_Pair)
				{
					if(CompareRfID(pRadioApi->RadioUdid, DevUdid, RfIdLength))
					{
						memcpy((void *)pRadioApi->RadioDevId, (void *)pRadioMsg->DeviceId, RfIdLength);
						pRadioApi->SelfDevNum = pRadioMsg->TargetNumber;

						HubSendAckMsg(HubNumber, eRfMsgPair, PairMsgAck, RadioMsgLeve2);
						RadioEventPair.sig =Q_eSystemEventPairStart;
						QEP_Post(&RadioEventPair);
					}
				}
				break;

			case PairMsgSuccess:
				if(States == eSystemState_Pair)
				{
					if(CompareRfID(pRadioApi->RadioDevId, pRadioMsg->DeviceId, RfIdLength) &&
												(CompareRfID(pRadioApi->RadioUdid, DevUdid, RfIdLength)))
					{
						HubSendAckMsg(HubNumber, eRfMsgPair, PairMsgAck, RadioMsgLeve2);

						pRadioApi->FatherDevNum = pRadioMsg->DeviceNumber;
						pRadioApi->FatherDevType = eTypeHub;

						RadioEventPair.sig = Q_eSystemEventPairSuccess;
						QEP_Post(&RadioEventPair);
					}
				}
				break;

			case PairMsgFail:
				break;

			default:
				break;
		}
	}
}

/**********************************************************************/
//Description:	HubRfHandleMsg()											
//Parameters:                        									
//Return:   															
//Date:           
//Author:		quanwu.xu  
/**********************************************************************/
void HubRfHandleMsg(uint8_t DataOffset)
{
	uint8_t RadioMsgLevel = 0;
	RadioMsgTag RfMsgTagObj = {0};
	RadioDevTag* pRadioApi = GetRfApiTag();

	memcpy(&RfMsgTagObj, (void *)&RadioReadBuf[DataOffset], HUB_PLAYLOAD_LEN);

	RadioMsgLevel = RfMsgTagObj.MsgLevel1;

	if(SystemApiTagObj.SystemState == eSystemState_Pair)
	{
		HubRfPairMsgHandle(&RfMsgTagObj, SystemApiTagObj.SystemState);
	}
	else if(CompareRfID(pRadioApi->RadioDevId, RfMsgTagObj.DeviceId, RfIdLength) &&
								((RfMsgTagObj.TargetNumber == pRadioApi->SelfDevNum) || 
											(RfMsgTagObj.TargetNumber == TxToAllFireAlarms)))
	{
		switch(RadioMsgLevel)
		{
			case eRfMsgWakeup:
				HubRfWakeupDevMsgHandle(&RfMsgTagObj);
				break;
					
			case eRfMsgTraverse:
				break;
		
			case eRfMsgDeleteDev:
				HubRfDeleteDevMsgHandle(&RfMsgTagObj);
				break;
		
			case eRfMsgSearchDev:
				HubRfSearchDevMsgHandle(&RfMsgTagObj);
				break;
			
			case eRfMsgHeartBeat:
				HubRfHeartbeatMsgHandle(&RfMsgTagObj);
				break;
		
			case eRfMsgFireAlarm:
				HubRfFireAlarmMsgHandle(&RfMsgTagObj);
				break;
		
			default:
				break;
		}
	}
}

/**********************************************************************/
//Description:	HubInitialAlarmMuteMode()															  
//Parameters:                        												  
//Return:   																  
//Date:           
//Author:		quanwu.xu  
/**********************************************************************/
void HubInitialAlarmMuteMode(SystemApiTag  *me,QEvent *e)
{
	static TmrHdl PollingEntryIdleModeHandle = NULL;
	RadioDevTag* pRadioApi = GetRfApiTag();

	debug_info("sig=%d\n",e->sig);
	switch (e->sig)
	{
		case Q_ENTRY_SIG:
			SetSysSleepFlag(eSysActiveFlag_AlarmMode);		
			me->SystemState = eSystemState_Alarm;
			PollingEntryIdleModeHandle = TmrInsertTimer(GetTmrHd(),
				TMR_PERIOD(Timeout1hour), PollingEntryIdleMode, NULL);
			break;

		case Q_EXIT_SIG:
			ResetSysSleepFlag(eSysActiveFlag_AlarmMode);
			me->SystemState = eSystemState_Idle;
			TmrDeleteTimer(PollingEntryIdleModeHandle);
			break;

		case Q_eUartEventUpdateAlarm:								//mcu update alarm
			me->ActiveAlarmFlag = true;
			HubRfUpdateHandleFunc(eRfMsgFireAlarm, e->Value1, e->Value2, e->Value3);
			Q_TRAN(HubInitialAlarmMode);
			break;

		case Q_eUartEventUpdateCoPpm:								//mcu update co ppm
			HubRfUpdateHandleFunc(eRfMsgFireAlarm, e->Value1, e->Value2, e->Value3);
			break;

		case Q_eUartEventUpdateCancelAlarm:							//mcu update cancel alarm
			HubRfUpdateHandleFunc(eRfMsgFireAlarm, e->Value1, e->Value2, e->Value3);
			Q_TRAN(SysIdleModeHandleFunc);
			break;

		case Q_eUartEventUpdateMute:								//mcu update mute
			HubRfUpdateHandleFunc(eRfMsgFireAlarm, e->Value1, e->Value2, e->Value3);
			break;

		case Q_eUartEventUpdateCancelMute:							//mcu update cancel mute
			HubRfUpdateHandleFunc(eRfMsgFireAlarm, e->Value1, e->Value2, e->Value3);
			break;

		case Q_eRadioEventDownloadAlarm:							//RF Alarm
			UartDownLoadHandleFunc(eUartMsgFireAlarm, e->Value1, e->Value2);
			Q_TRAN(HubAlarmMode);
			break;

		case Q_eRadioEventDownloadMute: 							//RF mute	(APP From)
			UartDownLoadHandleFunc(eUartMsgFireAlarm, e->Value1, e->Value2);
			break;

		case Q_eRadioEventDownloadCancelMute:
			UartDownLoadHandleFunc(eUartMsgFireAlarm, e->Value1, e->Value2);//RF Cancel Mute (APP From)
			break;

		case Q_eSystemEventLowpowerMode:							//20min timeout
			Q_TRAN(SysIdleModeHandleFunc);
			break;

		default:
			break;
	}
}

/**********************************************************************/
//Description:	HubInitialAlarmMode()															  
//Parameters:                        												  
//Return:   																  
//Date:           
//Author:		quanwu.xu  
/**********************************************************************/
void HubInitialAlarmMode(SystemApiTag  *me,QEvent *e)
{
	static TmrHdl PollingEntryIdleModeHandle = NULL;
	RadioDevTag* pRadioApi = GetRfApiTag();

	debug_info("sig=%d\n",e->sig);
	switch (e->sig)
	{
		case Q_ENTRY_SIG:
			SetSysSleepFlag(eSysActiveFlag_AlarmMode);
			me->SystemState = eSystemState_Alarm;
			me->ActiveAlarmFlag = true;
			PollingEntryIdleModeHandle = TmrInsertTimer(GetTmrHd(),
				TMR_PERIOD(Timeout1hour), PollingEntryIdleMode, NULL);
			break;

		case Q_EXIT_SIG:
			ResetSysSleepFlag(eSysActiveFlag_AlarmMode);
			me->SystemState = eSystemState_Idle;
			TmrDeleteTimer(PollingEntryIdleModeHandle);
			break;

		case Q_eUartEventUpdateCoPpm:							//mcu update co ppm
			HubRfUpdateHandleFunc(eRfMsgFireAlarm, e->Value1, e->Value2, e->Value3);
			break;

		case Q_eUartEventUpdateCancelAlarm:						//mcu update cancel alarm
			HubRfUpdateHandleFunc(eRfMsgFireAlarm, e->Value1, e->Value2, e->Value3);
			Q_TRAN(SysIdleModeHandleFunc);
			break;

		case Q_eUartEventUpdateMute:							//mcu update mute
			HubRfUpdateHandleFunc(eRfMsgFireAlarm, e->Value1, e->Value2, e->Value3);
			Q_TRAN(HubInitialAlarmMuteMode);
			break;

		case Q_eRadioEventDownloadMute: 						//RF mute	(APP From)
			UartDownLoadHandleFunc(eUartMsgFireAlarm, UartFireAlarmMsgMute, 0);
			Q_TRAN(HubInitialAlarmMuteMode);
			break;

		case Q_eSystemEventLowpowerMode:						//20min timeout
			Q_TRAN(SysIdleModeHandleFunc);
			break;

		default:
			break;
	}
}



/**********************************************************************/
//Description:	HubAlarmMode()															  
//Parameters:                        												  
//Return:   																  
//Date:           
//Author:		quanwu.xu  
/**********************************************************************/
void HubAlarmMode(SystemApiTag  *me,QEvent *e)
{
	static TmrHdl PollingEntryIdleModeHandle = NULL;
	RadioDevTag* pRadioApi = GetRfApiTag();

	debug_info("sig=%d\n",e->sig);
	switch (e->sig)
	{
		case Q_ENTRY_SIG:
			SetSysSleepFlag(eSysActiveFlag_AlarmMode);		
			me->SystemState = eSystemState_Alarm;
			PollingEntryIdleModeHandle = TmrInsertTimer(GetTmrHd(),
				TMR_PERIOD(Timeout1hour), PollingEntryIdleMode, NULL);
			break;

		case Q_EXIT_SIG:
			ResetSysSleepFlag(eSysActiveFlag_AlarmMode);
			me->SystemState = eSystemState_Idle;
			TmrDeleteTimer(PollingEntryIdleModeHandle);
			break;

		case Q_eUartEventUpdateCoPpm:							//mcu update co ppm
			HubRfUpdateHandleFunc(eRfMsgFireAlarm, e->Value1, e->Value2, e->Value3);
			break;

		case Q_eUartEventUpdateAlarm:							//mcu update alarm
			me->ActiveAlarmFlag = true;
			HubRfUpdateHandleFunc(eRfMsgFireAlarm, e->Value1, e->Value2, e->Value3);
			Q_TRAN(HubInitialAlarmMode);
			break;

		case Q_eUartEventUpdateCancelAlarm:						//mcu update cancel alarm
			HubRfUpdateHandleFunc(eRfMsgFireAlarm, e->Value1, e->Value2, e->Value3);
			Q_TRAN(SysIdleModeHandleFunc);
			break;

		case Q_eRadioEventDownloadCancelAlarm:					//RF cancel alarm
			UartDownLoadHandleFunc(eUartMsgFireAlarm, e->Value1, e->Value2);
			Q_TRAN(SysIdleModeHandleFunc);
			break;

		case Q_eSystemEventLowpowerMode:
			Q_TRAN(SysIdleModeHandleFunc);
			break;

		default:
			break;
	}
}



/**********************************************************************/
//Description:	HubRfUpdateHandleFunc()									
//Parameters:                        									
//Return:   															
//Date:           
//Author:		quanwu.xu  
/**********************************************************************/
void HubRfUpdateHandleFunc(eHubMsgType MsgLevel1, uint8_t MsgLevel2, 
											uint8_t Value1, uint8_t Value2)
{
	uint8_t Data3 = 0;
	uint8_t MsgProperty = RfMsgPropertyNeedAck;
	RadioDevTag* pRadioApi = GetRfApiTag();
	RfSendMsgParamer SendMsgParamer = {0};

	if(pRadioApi->FatherDevType == eTypeRepeater)
	{
		MsgProperty |= RfMsgPropertyNeedUpTransfer;
		Data3 = pRadioApi->SelfDevNum;
	}

	SetPlayloadBufDataPart1(&SendMsgParamer, pRadioApi->FatherDevNum, MsgLevel1,
												MsgLevel2, MsgProperty, PackNum_1);
	SetPlayloadBufDataPart2(&SendMsgParamer, Value1, Value2, Data3, 0, NormalHubMode);
	HubSendRadioMsg(&SendMsgParamer, true);

	while(RadioSendTagObject.TimerSendRfMsgHandle != NULL)
	{
		SystemPolling();
		RadioService();
		UartService();
		QEP_Handler((QFsm *)&SystemApiTagObj);

		if(RadioSendTagObject.RadioDownState == RadioDownState_DeleteMsgStop)
		{
			TmrDeleteTimer(RadioSendTagObject.TimerSendRfMsgHandle);
			RadioSendTagObject.TimerSendRfMsgHandle = NULL;
			break;
		}
	}
}

/**********************************************************************/
//Description:	HubPairToHubStatesHandle()								
//Parameters:															
//Return:																
//Date: 		  
//Author:	quanwu.xu  
/**********************************************************************/
void HubPairMode(SystemApiTag  *me,QEvent *e)
{
	static TmrHdl PairRequestTimeoutHeader = NULL;
	static TmrHdl PairTimeoutHeader = NULL;
	RadioDevTag* pRadioApi = GetRfApiTag();
	RfSendMsgParamer SendMsgParamer = {0};

	debug_info("sig=%d\n",e->sig);
	switch (e->sig)
	{
		case Q_ENTRY_SIG:
			Radio.RadioInit(NormalHubMode);
			SetSysSleepFlag(eSysActiveFlag_Pair);

			me->SystemState = eSystemState_Pair;
			PairRequestTimeoutHeader = TmrInsertTimer(GetTmrHd(),
						TMR_PERIOD(TimeOut3s), PollingPairRequest, NULL);

			PairTimeoutHeader = TmrInsertTimer(GetTmrHd(),
					TMR_PERIOD(TimeOut60s), PollingPairTimeout, NULL);
			break;

		case Q_EXIT_SIG:
			ResetSysSleepFlag(eSysActiveFlag_Pair);
			break;

		case Q_eSystemEventPairStart:
			TmrDeleteTimer(PairRequestTimeoutHeader);

			SetPlayloadBufDataPart1(&SendMsgParamer, HubNumber, eRfMsgPair, PairMsgStart, 
														RfMsgPropertyNeedAck, PackNum_1);
			SetPlayloadBufDataPart2(&SendMsgParamer, pRadioApi->RadioUdid[0], pRadioApi->RadioUdid[1], 
										pRadioApi->RadioUdid[2], pRadioApi->RadioUdid[3], NormalHubMode);

			HubSendRadioMsg(&SendMsgParamer, true);
			break;

		case Q_eSystemEventPairSuccess:
			UartDownLoadHandleFunc(eUartMsgPair, UartMsgPairSuccess, 0);
			break;

		case Q_eSystemEventPairFinished:
			SetPlayloadBufDataPart1(&SendMsgParamer, HubNumber, eRfMsgPair, PairMsgSuccess,
														RfMsgPropertyNeedAck, PackNum_1);
			SetPlayloadBufDataPart2(&SendMsgParamer, (uint8_t)pRadioApi->ProductType, 0, 0, 0, NormalHubMode);

			HubSendRadioMsg(&SendMsgParamer, true);
			break;

		case Q_eSystemEventPairFail:
		case Q_eIntranetEventPairStop:
			TmrDeleteTimer(PairRequestTimeoutHeader);
			TmrDeleteTimer(PairTimeoutHeader);
			Q_TRAN(SysIdleModeHandleFunc);
			break;
			
		default:
			break;
	}	
}

#ifdef SUPPORT_INTRANET_FUNCTION
#ifdef SUPPORT_SPEAK_LOCATION
uint8_t AlarmLocation = 0;
#endif
/**********************************************************************/
//Description:		IntranetSetRadioParamer()							
//Parameters:                        									
//Return:   															
//Date:           
//Author:			quanwu.xu  
/**********************************************************************/
void IntranetSetRadioParamer(IntranetSendMsgParamer *pSendMsgParamer, 
								eInternatMsgType MsgType,eRadioInitMode InitMode, 
													uint8_t PackNum, bool Flag)
{
	pSendMsgParamer->MsgType = MsgType;
	pSendMsgParamer->Mode  = InitMode;
	pSendMsgParamer->PackNum = PackNum;
}

/**********************************************************************/
//Description:		IntranetSend()															  
//Parameters:                        												  
//Return:   																  
//Date:           
//Author:			quanwu.xu  
/**********************************************************************/
void IntranetSend(IntranetRadioMsgTag *pRadioMsg, eRadioInitMode InitMode, uint16_t PackNum)
{
	uint8_t i = 0;	

	debug_printf("Tx:");
	for(i=0; i<Radio.PlayloadLen; i++)
	{
		debug_printf("%x ", ((uint8_t *)pRadioMsg)[i]);
	}
	debug_printf("\n");

	Radio.RadioInit(InitMode);
	
	Radio.RadioSend((uint8_t *)pRadioMsg, Radio.PlayloadLen, PackNum, 6000);
}


/**********************************************************************/
//Description:		IntranetSendRadioMsg()								
//Parameters:                        									
//Return:   															
//Date:           
//Author:			quanwu.xu  
/**********************************************************************/
void IntranetSendRadioMsg(IntranetSendMsgParamer *pSendMsgParamer, bool WaitDoneFlag)
{
	RadioDevTag* pRadioApi = GetRfApiTag();
	IntranetRadioMsgTag RfMsgTag = {0};
		
	memcpy((void*)RfMsgTag.DeviceId, (void *)pRadioApi->RadioDevId, RfIdLength);
	RfMsgTag.MsgType = pSendMsgParamer->MsgType;
	RfMsgTag.MsgCrc = GetCrc8((void*)&RfMsgTag, Radio.PlayloadLen - 1);

	IntranetSend(&RfMsgTag, pSendMsgParamer->Mode, pSendMsgParamer->PackNum);

	if(WaitDoneFlag == true)
	{
		Radio.WaitSendDone();
	}
}

/**********************************************************************/
//Description:		IntranetPollingPairFactory()						
//Parameters:                        									
//Return:   															
//Date:           
//Author:		quanwu.xu  
/**********************************************************************/
void IntranetPollingPairFactory(TmrHdl Handle, eTmrCmd eCmd)
{
	static uint8_t ReTxCnt = 0;
	IntranetSendMsgParamer SendMsgParamer = {0};

	Handle = Handle;	
	switch(eCmd)
	{
		case eInsertTimer:
			ReTxCnt = 0;
			break;
			
		case eTimeOut:
			if(ReTxCnt++ > 10)
			{
				SoftwareReset();
			}
			else
			{
				IntranetSetRadioParamer(&SendMsgParamer, eIntranetRadioMsgFactoryPairRequest, 
											FactoryPairMode, PackNum_1, false);
				IntranetSendRadioMsg(&SendMsgParamer, true);
			}
			break;
			
		default:
			break;
	}	
}

/**********************************************************************/
//Description:		IntranetPollingPairMaster()									
//Parameters:                        									
//Return:   															
//Date:           
//Author:		quanwu.xu  
/**********************************************************************/
void IntranetPollingPairMaster(TmrHdl Handle, eTmrCmd eCmd)
{
	static uint8_t TotalTimeCnt = 0;
	Handle = Handle;
	
	switch(eCmd)
	{
		case eInsertTimer:
			TotalTimeCnt = 0;
			break;

		case eTimeOut:
			TotalTimeCnt++;
			if(TotalTimeCnt > 60)
			{
				SoftwareReset();
			}

			break;
			
		default:
			break;
	}	
}


/**********************************************************************/
//Name：			IntranetPollingCancleTest()
//Description:															  
//Parameters:                        												  
//Return:   																  
//Date:           
//Author:		quanwu.xu  
/**********************************************************************/
static void IntranetPollingStartTest(TmrHdl Handle, eTmrCmd eCmd)
{
	QEvent PollingStartTestEvent = {0};
	Handle = Handle;	
	switch(eCmd)
	{
		case eTimeOut:
			PollingStartTestEvent.sig = Q_eIntranetEventStartEntryTestTimeout;
			QEP_Post(&PollingStartTestEvent);
			break;

		case eDelTimer:
			break;
			
		default:
			break;
	}
}


/**********************************************************************/
//Name：			IntranetPollingCancelTest()
//Description:															  
//Parameters:                        												  
//Return:   																  
//Date:           
//Author:		quanwu.xu  
/**********************************************************************/
static void IntranetPollingCancelTest(TmrHdl Handle, eTmrCmd eCmd)
{
	QEvent PollingCancelTestEvent = {0};
	Handle = Handle;	
	switch(eCmd)
	{
		case eTimeOut:
			PollingCancelTestEvent.sig = Q_eIntranetEventCancelTestTimeout;
			QEP_Post(&PollingCancelTestEvent);
			break;

		case eDelTimer:
			break;
			
		default:
			break;
	}
}


/**********************************************************************/
//Description:		IntranetPollingPairSlaver()										
//Parameters:                        									
//Return:   																  
//Date:           
//Author:		quanwu.xu  
/**********************************************************************/
void IntranetPollingPairSlaver(TmrHdl Handle, eTmrCmd eCmd)
{
	static uint8_t TimeCnt = 0;
	static uint8_t ReTxCnt = 0;
	IntranetSendMsgParamer SendMsgParamer = {0};

	Handle = Handle;	
	switch(eCmd)
	{
		case eInsertTimer:
			TimeCnt = 0;
			ReTxCnt = 0;
			break;

		case eTimeOut:
			TimeCnt++;

			if(TimeCnt < 10)
			{
				IntranetSetRadioParamer(&SendMsgParamer, eIntranetRadioMsgPairNewRequest, 
											NormalIntranetMode, PackNum_1, false);
				IntranetSendRadioMsg(&SendMsgParamer, true);
			}
			else if(TimeCnt == 10)
			{
				Radio.RadioInit(LowPowerIntranetMode);
			}
			else if(TimeCnt == 20)
			{
				TimeCnt = 0;
			}

			if(ReTxCnt++ > 60)
			{
				SoftwareReset();
			}
			break;
			
		default:
			break;
	}	
}

/**********************************************************************/
//Name：			IntranetPollingForwardTest()
//Description:															  
//Parameters:                        												  
//Return:   																  
//Date:           
//Author:		quanwu.xu  
/**********************************************************************/
void IntranetPollingForwardTest(TmrHdl Handle, eTmrCmd eCmd)
{
	QEvent PollingForwardTestEvent = {0};
	Handle = Handle;	
	switch(eCmd)
	{
		case eTimeOut:
			PollingForwardTestEvent.sig = Q_eIntranetEventForwardTestTimeout;
			QEP_Post(&PollingForwardTestEvent);
			break;

		case eDelTimer:
			break;
			
		default:
			break;
	}
}


/**********************************************************************/
//Name：			IntranetPollingForcedEntryLowpowerMode()
//Description:															
//Parameters:                        									
//Return:   															
//Date:           
//Author:		quanwu.xu  
/**********************************************************************/
void IntranetPollingForcedEntryLowpowerMode(TmrHdl Handle, eTmrCmd eCmd)
{
	QEvent PollingForceStopEvent = {0};
	Handle = Handle;	
	switch(eCmd)
	{
		case eTimeOut:
			PollingForceStopEvent.sig = Q_eIntranetEventEntryLowpowerMode;
			QEP_Post(&PollingForceStopEvent);
			break;

		case eDelTimer:
			break;
			
		default:
			break;
	}
}


/**********************************************************************/
//Name：			IntranetPollingStartAlarm()
//Description:															
//Parameters:                        									
//Return:   															
//Date:           
//Author:		quanwu.xu  
/**********************************************************************/
void IntranetPollingStartAlarm(TmrHdl Handle, eTmrCmd eCmd)
{
	QEvent PollingStartAlarm = {0};
	static uint8_t TimeCnt = 0;
	IntranetSendMsgParamer SendMsgParamer = {0};
	uint32_t Ctx = (uint32_t)TmrGetCtx(Handle);
	
	Handle = Handle;	
	switch(eCmd)
	{
		case eInsertTimer:
			TimeCnt = 0;
			break;
	
		case eTimeOut:
#ifdef SUPPORT_SPEAK_LOCATION
			IntranetSetRadioParamer(&SendMsgParamer, (eInternatMsgType)AlarmLocation, NormalIntranetMode, PackNum_2, false);
			IntranetSendRadioMsg(&SendMsgParamer, true);
			HalDelayMs(20);
#endif

			if(Ctx == SmokeAlarm)
			{
				IntranetSetRadioParamer(&SendMsgParamer, eIntranetRadioMsgSmokeAlarm, NormalIntranetMode, PackNum_1, false);
			}
			else if(Ctx == CoAlarm)
			{
				IntranetSetRadioParamer(&SendMsgParamer, eIntranetRadioMsgCoAlarm, NormalIntranetMode, PackNum_1, false);
			}
			IntranetSendRadioMsg(&SendMsgParamer, true);

			if(++TimeCnt>2)
			{
				TmrDeleteTimer(Handle);
				PollingStartAlarm.sig = Q_eIntranetEventStartEntryAlarmTimeout;
				QEP_Post(&PollingStartAlarm);
			}
			break;

		case eDelTimer:
			break;
			
		default:
			break;
	}
}


/**********************************************************************/
//Name：			IntranetPollingCancelTest()
//Description:															  
//Parameters:                        												  
//Return:   																  
//Date:           
//Author:		quanwu.xu  
/**********************************************************************/
void IntranetPollingCancelAlarm(TmrHdl Handle, eTmrCmd eCmd)
{
	QEvent PollingCancelTestEvent = {0};
	Handle = Handle;	
	switch(eCmd)
	{
		case eTimeOut:
			PollingCancelTestEvent.sig = Q_eIntranetEventCancelAlarmTimeout;
			QEP_Post(&PollingCancelTestEvent);
			break;

		case eDelTimer:
			break;
			
		default:
			break;
	}
}

/**********************************************************************/
//Name：			IntranetPollingForwardAlarm()
//Description:															  
//Parameters:                        												  
//Return:   																  
//Date:           
//Author:		quanwu.xu  
/**********************************************************************/
void IntranetPollingForwardAlarm(TmrHdl Handle, eTmrCmd eCmd)
{
	QEvent PollingForwardTestEvent = {0};
	Handle = Handle;	
	switch(eCmd)
	{
		case eTimeOut:
			PollingForwardTestEvent.sig = Q_eIntranetEventForwardAlarmTimeout;
			QEP_Post(&PollingForwardTestEvent);
			break;

		case eDelTimer:
			break;
			
		default:
			break;
	}
}


/**********************************************************************/
//Description:	IntranetPairFactoryModeEvent()								
//Parameters:                        									
//Return:   															
//Date:           
//Author:		quanwu.xu  
/**********************************************************************/
void IntranetPairFactoryModeEvent(SystemApiTag  *me,QEvent *e)
{
	static TmrHdl PollingPairFactoryHandle = NULL;
	RadioDevTag* pRadioApi = GetRfApiTag();
	IntranetSendMsgParamer SendMsgParamer = {0};

	debug_info("sig=%d\n",e->sig);
	switch (e->sig)
	{
		case Q_ENTRY_SIG:
			SetSysSleepFlag(eSysActiveFlag_Pair);		
			me->SystemState = eSystemState_Pair;

			Radio.RadioInit(FactoryPairMode);
			PollingPairFactoryHandle = TmrInsertTimer(GetTmrHd(), 
						TMR_PERIOD(TimeOut1s), IntranetPollingPairFactory, NULL);
			break;

		case Q_EXIT_SIG:
			ResetSysSleepFlag(eSysActiveFlag_Pair);
			me->SystemState = eSystemState_Idle;
			break;

		case Q_eIntranetEventPairStop:
			TmrDeleteTimer(PollingPairFactoryHandle);
			PollingPairFactoryHandle = NULL;
			Q_TRAN(&SysIdleModeHandleFunc);
			break;

		case Q_eIntranetEventPairAnswer:
			UartDownLoadHandleFunc(eUartMsgPair, UartMsgPairSuccess, 0);
			break;

		case Q_eSystemEventPairFinished:
			if(SaveIDProcess(pRadioApi->RadioDevId, true, ePairedIntranet) == true)
			{
				IntranetSetRadioParamer(&SendMsgParamer, eIntranetRadioMsgFactoryPairAck, 
										FactoryPairMode, PackNum_1, false);
				IntranetSendRadioMsg(&SendMsgParamer, true);

				HalDelayMs(50);
				SoftwareReset();
			}
			break;

		default:
			break;
	}
}


/**********************************************************************/
//Description:	IntranetPairMasterModeEvent()								
//Parameters:                        									
//Return:   															
//Date:           
//Author:		quanwu.xu  
/**********************************************************************/
void IntranetPairMasterModeEvent(SystemApiTag  *me,QEvent *e)
{
	static TmrHdl PollingPairMasterHandle = NULL;
	RadioDevTag* pRadioApi = GetRfApiTag();
	IntranetSendMsgParamer SendMsgParamer = {0};

	debug_info("sig=%d\n",e->sig);
	switch (e->sig)
	{
		case Q_ENTRY_SIG:
			SetSysSleepFlag(eSysActiveFlag_Pair);		
			me->SystemState = eSystemState_Pair;

			Radio.RadioInit(NormalIntranetMode);
			PollingPairMasterHandle = TmrInsertTimer(GetTmrHd(), 
						TMR_PERIOD(TimeOut1s), IntranetPollingPairMaster, NULL);
			break;

		case Q_EXIT_SIG:
			ResetSysSleepFlag(eSysActiveFlag_Pair);
			me->SystemState = eSystemState_Idle;
			break;

		case Q_eIntranetEventPairStop:
			TmrDeleteTimer(PollingPairMasterHandle);
			PollingPairMasterHandle = NULL;
			Q_TRAN(&SysIdleModeHandleFunc);
			break;

		case Q_eIntranetEventPairRequest:
			if(pRadioApi->PairMode == ePairedNull)
			{
				memcpy((void *)(pRadioApi->RadioDevId), (void *)(pRadioApi->RadioUdid), RfIdLength);
			}
		
			IntranetSetRadioParamer(&SendMsgParamer, eIntranetRadioMsgPairNewPairAck, 
										NormalIntranetMode, PackNum_1, false);
			IntranetSendRadioMsg(&SendMsgParamer, true);

			if(SaveIDProcess(pRadioApi->RadioDevId, true, ePairedIntranet) == true)
			{
				UartDownLoadHandleFunc(eUartMsgPair, UartMsgPairSuccess, 0);
				HalDelayMs(50);
			}
			SoftwareReset();
			break;

		default:
			break;
	}
}


/**********************************************************************/
//Description:	SysPairSlaverModeEvent()								
//Parameters:                        									
//Return:   															
//Date:           
//Author:		quanwu.xu  
/**********************************************************************/
void IntranetPairSlaverModeEvent(SystemApiTag  *me,QEvent *e)
{
	static TmrHdl PollingPairSlaverHandle = NULL;
	RadioDevTag* pRadioApi = GetRfApiTag();
	IntranetSendMsgParamer SendMsgParamer = {0};

	debug_info("sig=%d\n",e->sig);
	switch (e->sig)
	{
		case Q_ENTRY_SIG:
			SetSysSleepFlag(eSysActiveFlag_Pair);		
			me->SystemState = eSystemState_Pair;

			PollingPairSlaverHandle = TmrInsertTimer(GetTmrHd(),
							TMR_PERIOD(TimeOut1s), IntranetPollingPairSlaver, NULL);
			break;

		case Q_EXIT_SIG:
			ResetSysSleepFlag(eSysActiveFlag_Pair);
			me->SystemState = eSystemState_Idle;
			break;

		case Q_eIntranetEventPairStop:
			TmrDeleteTimer(PollingPairSlaverHandle);
			PollingPairSlaverHandle = NULL;
			Q_TRAN(&SysIdleModeHandleFunc);
			break;

		case Q_eIntranetEventPairAnswer:
		case Q_eIntranetEventPairOldRequest:
			UartDownLoadHandleFunc(eUartMsgPair, UartMsgPairSuccess, 0);
			break;
		
		case Q_eSystemEventPairFinished:
			if(SaveIDProcess(pRadioApi->RadioDevId, true, ePairedIntranet) == true)
			{

				IntranetSetRadioParamer(&SendMsgParamer, eIntranetRadioMsgPairAck, 
										NormalIntranetMode, PackNum_1, false);
				IntranetSendRadioMsg(&SendMsgParamer, true);

				HalDelayMs(50);
				SoftwareReset();
			}
			break;

		default:
			break;
	}
}


/**********************************************************************/
//Description:	IntranetTestModeEvent()										
//Parameters:                        									
//Return:   															
//Date:           
//Author:		quanwu.xu  
/**********************************************************************/
void IntranetTestModeEvent(SystemApiTag  *me,QEvent *e)
{
	RadioDevTag* pRadioApi = GetRfApiTag();
	IntranetSendMsgParamer SendMsgParamer = {0};
	static TmrHdl PollingForceEntryLowpowerModeHandle = NULL;

	debug_info("sig=%d\n",e->sig);
	switch (e->sig)
	{
		case Q_ENTRY_SIG:
			SetSysSleepFlag(eSysActiveFlag_TestMode);
			me->SystemState = eSystemState_Test;

			IntranetSetRadioParamer(&SendMsgParamer, eIntranetRadioMsgMasterTest, NormalIntranetMode, PackNum_32, false);
			IntranetSendRadioMsg(&SendMsgParamer, true);

#if defined(SUPPORT_X_SENSE_EN) || defined(SUPPORT_X_SENSE_UL)
			IntranetSetRadioParamer(&SendMsgParamer, eIntranetRadioMsgMasterTest, NormalIntranetSpecialMode, PackNum_2, false);
			IntranetSendRadioMsg(&SendMsgParamer, true);
#endif

			IntranetSetRadioParamer(&SendMsgParamer, eIntranetRadioMsgSync, NormalIntranetMode, PackNum_1, false);
			IntranetSendRadioMsg(&SendMsgParamer, true);

			IntranetSetRadioParamer(&SendMsgParamer, eIntranetRadioMsgMasterTest, LowPowerIntranetMode, PackNum_1, false);
			IntranetSendRadioMsg(&SendMsgParamer, true);
			PollingForceEntryLowpowerModeHandle = TmrInsertTimer(GetTmrHd(),
											TMR_PERIOD(TimeOut180s), IntranetPollingForcedEntryLowpowerMode, NULL);
			break;

		case Q_EXIT_SIG:
			ResetSysSleepFlag(eSysActiveFlag_TestMode);
			me->SystemState = eSystemState_Idle;

			TmrDeleteTimer(PollingForceEntryLowpowerModeHandle);
			PollingForceEntryLowpowerModeHandle = NULL;
			break;

		case Q_eIntranetEventBroadcastCancelTest:
			Q_TRAN(&IntranetBroadcastCancelTestModeEvent);
			break;

		case Q_eIntranetEventMasterTest:
			Q_TRAN(&IntranetForwardTestModeEvent);
			break;
	
		case Q_eIntranetEventBroadcastTest:
			Q_TRAN(&IntranetBroadcastTestModeEvent);
			break;

		case Q_eIntranetEventEntryLowpowerMode:
			UartDownLoadHandleFunc(eUartMsgFireAlarm, UartFireAlarmMsgTest, DevTestStop);

			Q_TRAN(&SysIdleModeHandleFunc);
			break;

		case Q_eIntranetEventCancelSync:
			if(me->SystemState == eSystemState_CancelTest)
			{
				IntranetSetRadioParamer(&SendMsgParamer, eIntranetRadioMsgSync, NormalIntranetMode, PackNum_4, false);
				IntranetSendRadioMsg(&SendMsgParamer, true);
				HalDelayMs(2);
				IntranetSetRadioParamer(&SendMsgParamer, eIntranetRadioMsgCancleTest, NormalIntranetMode, PackNum_4, false);
				IntranetSendRadioMsg(&SendMsgParamer, true);
			}
			break;
			
		default:
			break;
	}
}

/**********************************************************************/
//Description:	IntranetBroadcastTestModeEvent()										
//Parameters:                        									
//Return:   															
//Date:           
//Author:		quanwu.xu  
/**********************************************************************/
void IntranetBroadcastTestModeEvent(SystemApiTag  *me,QEvent *e)
{
	static TmrHdl PollingStartTestHandle = NULL;
	IntranetSendMsgParamer SendMsgParamer = {0};

	debug_info("sig=%d\n",e->sig);
	switch (e->sig)
	{
		case Q_ENTRY_SIG:
			SetSysSleepFlag(eSysActiveFlag_TestMode);
			me->SystemState = eSystemState_Test;
			
			Radio.RadioInit(NormalIntranetMode);
			PollingStartTestHandle = TmrInsertTimer(GetTmrHd(),
											TMR_PERIOD(TimeOut1s), IntranetPollingStartTest, NULL);
			break;

		case Q_EXIT_SIG:
			ResetSysSleepFlag(eSysActiveFlag_TestMode);
			me->SystemState = eSystemState_Idle;
			TmrDeleteTimer(PollingStartTestHandle);
			PollingStartTestHandle = NULL;
		break;

		case Q_eIntranetEventBroadcastTest:
			Q_TRAN(&IntranetBroadcastTestModeEvent);
			break;

		case Q_eIntranetEventBroadcastCancelTest:
			Q_TRAN(&IntranetBroadcastCancelTestModeEvent);
			break;

		case Q_eIntranetEventStartEntryTestTimeout:
			Q_TRAN(&IntranetTestModeEvent);
			break;

		case Q_eIntranetEventCancelTest:
			IntranetSetRadioParamer(&SendMsgParamer, eIntranetRadioMsgCancleMsgSync, NormalIntranetMode, PackNum_32, false);
			IntranetSendRadioMsg(&SendMsgParamer, true);
			HalDelayMs(1000);
			TmrDeleteTimer(PollingStartTestHandle);
			PollingStartTestHandle = NULL;
			
			PollingStartTestHandle = TmrInsertTimer(GetTmrHd(),
										TMR_PERIOD(TimeOut1s), IntranetPollingStartTest, NULL);
			break;
			
		default:
			break;
	}
}


/**********************************************************************/
//Description:	IntranetsBroadcastCancelTestModeEvent()										
//Parameters:                        									
//Return:   															
//Date:           
//Author:		quanwu.xu  
/**********************************************************************/
void IntranetBroadcastCancelTestModeEvent(SystemApiTag  *me,QEvent *e)
{
	static TmrHdl PollingCancelTestHandle = NULL;
	static uint8_t TimeCnt1 = 0;
	static uint8_t TimeCnt2 = 0;
	IntranetSendMsgParamer SendMsgParamer = {0};

	debug_info("sig=%d\n",e->sig);
	switch (e->sig)
	{
		case Q_ENTRY_SIG:
			TimeCnt1 = 0;
			TimeCnt2 = 0;
			
			SetSysSleepFlag(eSysActiveFlag_CancelMode);
			me->SystemState = eSystemState_CancelTest;

			IntranetSetRadioParamer(&SendMsgParamer, eIntranetRadioMsgSync, NormalIntranetMode, PackNum_32, false);
			IntranetSendRadioMsg(&SendMsgParamer, true);

			HalDelayMs(2);

			IntranetSetRadioParamer(&SendMsgParamer, eIntranetRadioMsgCancleTest, NormalIntranetMode, PackNum_16, false);
			IntranetSendRadioMsg(&SendMsgParamer, true);
			
			PollingCancelTestHandle = TmrInsertTimer(GetTmrHd(),
											TMR_PERIOD(TimeOut5s), IntranetPollingCancelTest, NULL);
			break;

		case Q_EXIT_SIG:
			ResetSysSleepFlag(eSysActiveFlag_CancelMode);
			me->SystemState = eSystemState_Idle;
			TmrDeleteTimer(PollingCancelTestHandle);
			PollingCancelTestHandle = NULL;
			break;

		case Q_eIntranetEventBroadcastTest:
			Q_TRAN(&IntranetBroadcastTestModeEvent);
			break;

		case Q_eIntranetEventMasterTest:
			Q_TRAN(&IntranetForwardTestModeEvent);
			break;

		case Q_eIntranetEventCancelTestTimeout:
			IntranetSetRadioParamer(&SendMsgParamer, eIntranetRadioMsgCancleTest, NormalIntranetMode, PackNum_4, false);
			IntranetSendRadioMsg(&SendMsgParamer, true);
			if(TimeCnt1++ > 2)
			{
				Q_TRAN(&SysIdleModeHandleFunc);
			}
			break;

		case Q_eIntranetEventCancelSync:
			IntranetSetRadioParamer(&SendMsgParamer, eIntranetRadioMsgSync, NormalIntranetMode, PackNum_4, false);
			IntranetSendRadioMsg(&SendMsgParamer, true);
			HalDelayMs(2);
			IntranetSetRadioParamer(&SendMsgParamer, eIntranetRadioMsgCancleTest, NormalIntranetMode, PackNum_4, false);
			IntranetSendRadioMsg(&SendMsgParamer, true);
			if(TimeCnt2++ > 2)
			{
				Q_TRAN(&SysIdleModeHandleFunc);
			}
			break;

		case Q_eIntranetEventPairIntranetMasterMode:
			Q_TRAN(IntranetPairMasterModeEvent);
			break;
		
		case Q_eIntranetEventPairIntranetSlaverMode:
			Q_TRAN(IntranetPairSlaverModeEvent);
			break;

		default:
			break;
	}
}

/**********************************************************************/
//Description:	IntranetBroadcastSmokeAlarmModeEvent()										
//Parameters:                        									
//Return:   															
//Date:           
//Author:		quanwu.xu  
/**********************************************************************/
void IntranetBroadcastSmokeAlarmModeEvent(SystemApiTag  *me,QEvent *e)
{
	IntranetSendMsgParamer SendMsgParamer = {0};
	static TmrHdl PollingStartAlarmHandle = NULL;
	static TmrHdl PollingForceEntryLowpowerModeHandle = NULL;

	debug_info("sig=%d\n",e->sig);
	switch (e->sig)
	{
		case Q_ENTRY_SIG:
			SetSysSleepFlag(eSysActiveFlag_AlarmMode);
			me->SystemState = eSystemState_Alarm;
			
			IntranetSetRadioParamer(&SendMsgParamer, eIntranetRadioMsgSmokeAlarm, LowPowerIntranetMode, PackNum_1, false);
			IntranetSendRadioMsg(&SendMsgParamer, true);

			PollingStartAlarmHandle = TmrInsertTimer(GetTmrHd(),
											TMR_PERIOD(TimeOut200ms), IntranetPollingStartAlarm, (void *)(SmokeAlarm));

			PollingForceEntryLowpowerModeHandle = TmrInsertTimer(GetTmrHd(),
											TMR_PERIOD(Timeout1hour), IntranetPollingForcedEntryLowpowerMode, NULL);
			break;

		case Q_EXIT_SIG:
			ResetSysSleepFlag(eSysActiveFlag_AlarmMode);
			me->SystemState = eSystemState_Idle;
			TmrDeleteTimer(PollingStartAlarmHandle);
			break;

		case Q_eIntranetEventBroadcastMute:
		case Q_eIntranetEventBroadcastCancelAlarm:
			Q_TRAN(&IntranetBroadcastCancelAlarmModeEvent);
			break;

		case Q_eIntranetEventEntryLowpowerMode:
			TmrDeleteTimer(PollingForceEntryLowpowerModeHandle);
			PollingForceEntryLowpowerModeHandle = NULL;
			Q_TRAN(&SysIdleModeHandleFunc);
			break;

		default:
			break;
	}
}



/**********************************************************************/
//Description:	IntranetBroadcastCoAlarmModeEvent()										
//Parameters:                        									
//Return:   															
//Date:           
//Author:		quanwu.xu  
/**********************************************************************/
void IntranetBroadcastCoAlarmModeEvent(SystemApiTag  *me,QEvent *e)
{
	IntranetSendMsgParamer SendMsgParamer = {0};
	static TmrHdl PollingStartAlarmHandle = NULL;
	static TmrHdl PollingForceEntryLowpowerModeHandle = NULL;

	debug_info("sig=%d\n",e->sig);
	switch (e->sig)
	{
		case Q_ENTRY_SIG:
			SetSysSleepFlag(eSysActiveFlag_AlarmMode);
			me->SystemState = eSystemState_Alarm;
			
			IntranetSetRadioParamer(&SendMsgParamer, eIntranetRadioMsgCoAlarm, LowPowerIntranetMode, PackNum_1, false);
			IntranetSendRadioMsg(&SendMsgParamer, true);

			PollingStartAlarmHandle = TmrInsertTimer(GetTmrHd(),
											TMR_PERIOD(TimeOut200ms), IntranetPollingStartAlarm, (void *)(CoAlarm));

			PollingForceEntryLowpowerModeHandle = TmrInsertTimer(GetTmrHd(),
											TMR_PERIOD(Timeout1hour), IntranetPollingForcedEntryLowpowerMode, NULL);
			break;

		case Q_EXIT_SIG:
			ResetSysSleepFlag(eSysActiveFlag_AlarmMode);
			me->SystemState = eSystemState_Idle;
			TmrDeleteTimer(PollingStartAlarmHandle);
			PollingStartAlarmHandle = NULL;
			break;

		case Q_eIntranetEventBroadcastMute:
		case Q_eIntranetEventBroadcastCancelAlarm:
			Q_TRAN(&IntranetBroadcastCancelAlarmModeEvent);
			break;

		case Q_eIntranetEventEntryLowpowerMode:
			TmrDeleteTimer(PollingForceEntryLowpowerModeHandle);
			PollingForceEntryLowpowerModeHandle = NULL;
			Q_TRAN(&SysIdleModeHandleFunc);
			break;

		default:
			break;
	}
}

/**********************************************************************/
//Description:	IntranetBroadcastCancelAlarmModeEvent()										
//Parameters:                        									
//Return:   															
//Date:           
//Author:		quanwu.xu  
/**********************************************************************/
void IntranetBroadcastCancelAlarmModeEvent(SystemApiTag  *me,QEvent *e)
{
	uint16_t TimeOutCnt = 0;
	IntranetSendMsgParamer SendMsgParamer = {0};
	static TmrHdl PollingStartCancelAlarmHandle = NULL;
	static uint8_t TimeCnt = 0;

	debug_info("sig=%d\n",e->sig);
	switch (e->sig)
	{
		case Q_ENTRY_SIG:
			SetSysSleepFlag(eSysActiveFlag_CancelMode);
			me->SystemState = eSystemState_CancelAlarm;

			TimeCnt = 0;

			IntranetSetRadioParamer(&SendMsgParamer, eIntranetRadioMsgCancleAlarm, NormalIntranetMode, PackNum_8, false);
			IntranetSendRadioMsg(&SendMsgParamer, true);

			TimeOutCnt = GetRandDelayTime(eNormalIntranetMode);
			PollingStartCancelAlarmHandle = TmrInsertTimer(GetTmrHd(),
									TMR_PERIOD(TimeOutCnt + TX_DELAY_LONG_TIME), IntranetPollingCancelAlarm, NULL);
			break;

		case Q_EXIT_SIG:
			ResetSysSleepFlag(eSysActiveFlag_CancelMode);
			me->SystemState = eSystemState_Idle;
			TmrDeleteTimer(PollingStartCancelAlarmHandle);
			PollingStartCancelAlarmHandle = NULL;
			break;

		case Q_eIntranetEventBroadcastTest:
			Q_TRAN(&IntranetBroadcastTestModeEvent);
			break;
			
		case Q_eIntranetEventMasterTest:
			Q_TRAN(&IntranetForwardTestModeEvent);
			break;

		case Q_eIntranetEventBroadcastSmokeAlarm:
			Q_TRAN(&IntranetBroadcastSmokeAlarmModeEvent);
			break;
		
		case Q_eIntranetEventBroadcastCoAlarm:
			Q_TRAN(&IntranetBroadcastCoAlarmModeEvent);
			break;

		case Q_eIntranetEventCancelAlarmTimeout:
			IntranetSetRadioParamer(&SendMsgParamer, eIntranetRadioMsgSync, NormalIntranetMode, PackNum_4, false);
			IntranetSendRadioMsg(&SendMsgParamer, true);
			HalDelayMs(2);

			IntranetSetRadioParamer(&SendMsgParamer, eIntranetRadioMsgCancleAlarm, NormalIntranetMode, PackNum_2, false);
			IntranetSendRadioMsg(&SendMsgParamer, true);
			if(TimeCnt++ > 2)
			{
				Q_TRAN(&SysIdleModeHandleFunc);
			}
			break;

		default:
			break;
	}
}

/**********************************************************************/
//Description:	IntranetForwardSmokeAlarmModeEvent()										
//Parameters:                        									
//Return:   															
//Date:           
//Author:		quanwu.xu  
/**********************************************************************/
void IntranetForwardSmokeAlarmModeEvent(SystemApiTag  *me,QEvent *e)
{
	uint16_t TimeOutCnt = 0;
	RadioDevTag* pRadioApi = GetRfApiTag();
	IntranetSendMsgParamer SendMsgParamer = {0};
	static TmrHdl PollingForwardAlarmHandle = NULL;
#ifdef SUPPORT_SPEAK_LOCATION
	static eInternatMsgType RaidoLocation = eIntranetRadioMsgNull;
#endif

	debug_info("sig=%d\n",e->sig);
	switch (e->sig)
	{
		case Q_ENTRY_SIG:
#ifdef SUPPORT_SPEAK_LOCATION
			RaidoLocation = eIntranetRadioMsgNull;
#endif
			SetSysSleepFlag(eSysActiveFlag_AlarmMode);
			me->SystemState = eSystemState_Alarm;
			Radio.RadioInit(NormalIntranetMode);

			UartDownLoadHandleFunc(eUartMsgFireAlarm, UartFireAlarmMsgSmokeAlarm, 0);

			TimeOutCnt = GetRandDelayTime(eLowpowerIntranetMode);
			PollingForwardAlarmHandle = TmrInsertTimer(GetTmrHd(),
					TMR_PERIOD(TimeOutCnt + TX_DELAY_LONG_TIME), IntranetPollingForwardAlarm, NULL);
			break;

		case Q_EXIT_SIG:
			ResetSysSleepFlag(eSysActiveFlag_AlarmMode);
			me->SystemState = eSystemState_Idle;

			TmrDeleteTimer(PollingForwardAlarmHandle);
			PollingForwardAlarmHandle = NULL;
			break;

		case Q_eIntranetEventBroadcastSmokeAlarm:
			Q_TRAN(&IntranetBroadcastSmokeAlarmModeEvent);
			break;
		
		case Q_eIntranetEventBroadcastCoAlarm:
			Q_TRAN(&IntranetBroadcastCoAlarmModeEvent);
			break;

		case Q_eIntranetEventBroadcastCancelAlarm:
		case Q_eIntranetEventBroadcastMute:
			Q_TRAN(&IntranetBroadcastCancelAlarmModeEvent);
			break;

		case Q_eIntranetEventCancelAlarm:
			Q_TRAN(&IntranetForwardCancelAlarmModeEvent);
			break;

		case Q_eIntranetEventSync:
			TmrDeleteTimer(PollingForwardAlarmHandle);
			PollingForwardAlarmHandle = NULL;

			TimeOutCnt = GetRandDelayTime(eLowpowerIntranetMode);
			PollingForwardAlarmHandle = TmrInsertTimer(GetTmrHd(),
					TMR_PERIOD(TimeOutCnt + TX_DELAY_LONG_TIME), IntranetPollingForwardAlarm, NULL);
			break;
			
#ifdef SUPPORT_SPEAK_LOCATION
		case Q_eIntranetEventAlarmLocation:
			RaidoLocation = (eInternatMsgType)e->Value1;
			UartDownLoadHandleFunc(eUartMsgFireAlarm, UartFireAlarmMsgSmokeAlarm, (uint8_t)RaidoLocation);
			break;
#endif

		case Q_eIntranetEventForwardAlarmTimeout:
			TmrDeleteTimer(PollingForwardAlarmHandle);
			PollingForwardAlarmHandle = NULL;

			IntranetSetRadioParamer(&SendMsgParamer, eIntranetRadioMsgSync, NormalIntranetMode, PackNum_1, false);
			IntranetSendRadioMsg(&SendMsgParamer, true);

			IntranetSetRadioParamer(&SendMsgParamer, eIntranetRadioMsgSmokeAlarm, LowPowerIntranetMode, PackNum_1, false);
			IntranetSendRadioMsg(&SendMsgParamer, true);

			IntranetSetRadioParamer(&SendMsgParamer, eIntranetRadioMsgCancleMsgSync, NormalIntranetMode, PackNum_2, false);
			IntranetSendRadioMsg(&SendMsgParamer, true);

#ifdef SUPPORT_SPEAK_LOCATION
			IntranetSetRadioParamer(&SendMsgParamer, RaidoLocation, NormalIntranetMode, PackNum_2, false);
			IntranetSendRadioMsg(&SendMsgParamer, true);
#endif
			Q_TRAN(&IntranetAlarmModeEvent);
			break;

		default:
			break;
	}
}


/**********************************************************************/
//Description:	IntranetAlarmModeEvent()										
//Parameters:                        									
//Return:   															
//Date:           
//Author:		quanwu.xu  
/**********************************************************************/
void IntranetAlarmModeEvent(SystemApiTag  *me,QEvent *e)
{
	RadioDevTag* pRadioApi = GetRfApiTag();
	static TmrHdl PollingForceEntryLowpowerModeHandle = NULL;

	debug_info("sig=%d\n",e->sig);
	switch (e->sig)
	{
		case Q_ENTRY_SIG:
			SetSysSleepFlag(eSysActiveFlag_AlarmMode);
			me->SystemState = eSystemState_Alarm;
			
			PollingForceEntryLowpowerModeHandle = TmrInsertTimer(GetTmrHd(),
											TMR_PERIOD(Timeout1hour), IntranetPollingForcedEntryLowpowerMode, NULL);
			break;

		case Q_EXIT_SIG:
			ResetSysSleepFlag(eSysActiveFlag_AlarmMode);
			me->SystemState = eSystemState_Idle;

			TmrDeleteTimer(PollingForceEntryLowpowerModeHandle);
			PollingForceEntryLowpowerModeHandle = NULL;
			break;

		case Q_eIntranetEventEntryLowpowerMode:
			Q_TRAN(&SysIdleModeHandleFunc);
			break;

		case Q_eIntranetEventBroadcastCancelAlarm:
		case Q_eIntranetEventBroadcastMute:
			Q_TRAN(&IntranetBroadcastCancelAlarmModeEvent);
			break;

		case Q_eIntranetEventCancelAlarm:
			Q_TRAN(&IntranetForwardCancelAlarmModeEvent);
			break;

		default:
			break;
	}
}

/**********************************************************************/
//Description:	IntranetForwardAlarmModeEvent()							
//Parameters:                        									
//Return:   															
//Date:           
//Author:		quanwu.xu  
/**********************************************************************/
void IntranetForwardCoAlarmModeEvent(SystemApiTag  *me,QEvent *e)
{
	uint16_t TimeOutCnt = 0;
	RadioDevTag* pRadioApi = GetRfApiTag();
	IntranetSendMsgParamer SendMsgParamer = {0};
	static TmrHdl PollingForwardAlarmHandle = NULL;
#ifdef SUPPORT_SPEAK_LOCATION
	static eInternatMsgType RaidoLocation = eIntranetRadioMsgNull;
#endif

	debug_info("sig=%d\n",e->sig);
	switch (e->sig)
	{
		case Q_ENTRY_SIG:
#ifdef SUPPORT_SPEAK_LOCATION
			RaidoLocation = eIntranetRadioMsgNull;
#endif
			SetSysSleepFlag(eSysActiveFlag_AlarmMode);
			me->SystemState = eSystemState_Alarm;
			Radio.RadioInit(NormalIntranetMode);

			UartDownLoadHandleFunc(eUartMsgFireAlarm, UartFireAlarmMsgCoAlarm, 0);

			TimeOutCnt = GetRandDelayTime(eLowpowerIntranetMode);
			PollingForwardAlarmHandle = TmrInsertTimer(GetTmrHd(),
					TMR_PERIOD(TimeOutCnt + TX_DELAY_LONG_TIME), IntranetPollingForwardAlarm, NULL);
			break;

		case Q_EXIT_SIG:
			ResetSysSleepFlag(eSysActiveFlag_AlarmMode);
			me->SystemState = eSystemState_Idle;

			TmrDeleteTimer(PollingForwardAlarmHandle);
			PollingForwardAlarmHandle = NULL;
			break;

		case Q_eIntranetEventBroadcastSmokeAlarm:
			Q_TRAN(&IntranetBroadcastSmokeAlarmModeEvent);
			break;
		
		case Q_eIntranetEventBroadcastCoAlarm:
			Q_TRAN(&IntranetBroadcastCoAlarmModeEvent);
			break;

		case Q_eIntranetEventBroadcastCancelAlarm:
		case Q_eIntranetEventBroadcastMute:
			Q_TRAN(&IntranetBroadcastCancelAlarmModeEvent);
			break;

		case Q_eIntranetEventCancelAlarm:
			Q_TRAN(&IntranetForwardCancelAlarmModeEvent);
			break;

		case Q_eIntranetEventSync:
			TmrDeleteTimer(PollingForwardAlarmHandle);
			PollingForwardAlarmHandle = NULL;

			TimeOutCnt = GetRandDelayTime(eLowpowerIntranetMode);
			PollingForwardAlarmHandle = TmrInsertTimer(GetTmrHd(),
					TMR_PERIOD(TimeOutCnt + TX_DELAY_LONG_TIME), IntranetPollingForwardAlarm, NULL);
			break;

#ifdef SUPPORT_SPEAK_LOCATION
		case Q_eIntranetEventAlarmLocation:
			RaidoLocation = (eInternatMsgType)e->Value1;
			UartDownLoadHandleFunc(eUartMsgFireAlarm, UartFireAlarmMsgSmokeAlarm, (uint8_t)RaidoLocation);
			break;
#endif

		case Q_eIntranetEventForwardAlarmTimeout:
			TmrDeleteTimer(PollingForwardAlarmHandle);
			PollingForwardAlarmHandle = NULL;

			IntranetSetRadioParamer(&SendMsgParamer, eIntranetRadioMsgSync, NormalIntranetMode, PackNum_1, false);
			IntranetSendRadioMsg(&SendMsgParamer, true);

			IntranetSetRadioParamer(&SendMsgParamer, eIntranetRadioMsgCoAlarm, LowPowerIntranetMode, PackNum_1, false);
			IntranetSendRadioMsg(&SendMsgParamer, true);

			IntranetSetRadioParamer(&SendMsgParamer, eIntranetRadioMsgCancleMsgSync, NormalIntranetMode, PackNum_2, false);
			IntranetSendRadioMsg(&SendMsgParamer, true);

#ifdef SUPPORT_SPEAK_LOCATION
			IntranetSetRadioParamer(&SendMsgParamer, RaidoLocation, NormalIntranetMode, PackNum_2, false);
			IntranetSendRadioMsg(&SendMsgParamer, true);
#endif

			Q_TRAN(&IntranetAlarmModeEvent);
			break;

		default:
			break;
	}
}

/**********************************************************************/
//Description:	IntranetForwardCancelAlarmModeEvent()					
//Parameters:                        									
//Return:   															
//Date:           
//Author:		quanwu.xu  
/**********************************************************************/
void IntranetForwardCancelAlarmModeEvent(SystemApiTag  *me,QEvent *e)
{
	uint16_t TimeOutCnt = 0;
	RadioDevTag* pRadioApi = GetRfApiTag();
	IntranetSendMsgParamer SendMsgParamer = {0};
	static TmrHdl PollingForwardCancelAlarmHandle = NULL;

	debug_info("sig=%d\n",e->sig);
	switch (e->sig)
	{
		case Q_ENTRY_SIG:
			SetSysSleepFlag(eSysActiveFlag_CancelMode);
			me->SystemState = eSystemState_CancelAlarm;

			UartDownLoadHandleFunc(eUartMsgFireAlarm, UartFireAlarmMsgCancelAlarm, 0);

			TimeOutCnt = GetRandDelayTime(eLowpowerIntranetMode);
			PollingForwardCancelAlarmHandle = TmrInsertTimer(GetTmrHd(),
							TMR_PERIOD(TimeOutCnt + TX_DELAY_SHORT_TIME), IntranetPollingCancelAlarm, NULL);
			break;

		case Q_EXIT_SIG:
			ResetSysSleepFlag(eSysActiveFlag_CancelMode);
			me->SystemState = eSystemState_Idle;

			TmrDeleteTimer(PollingForwardCancelAlarmHandle);
			PollingForwardCancelAlarmHandle = NULL;
			break;

		case Q_eIntranetEventBroadcastTest:
			Q_TRAN(&IntranetBroadcastTestModeEvent);
			break;

		case Q_eIntranetEventMasterTest:
			Q_TRAN(&IntranetForwardTestModeEvent);
			break;

		case Q_eIntranetEventBroadcastSmokeAlarm:
			Q_TRAN(&IntranetBroadcastSmokeAlarmModeEvent);
			break;
		
		case Q_eIntranetEventBroadcastCoAlarm:
			Q_TRAN(&IntranetBroadcastCoAlarmModeEvent);
			break;

		case Q_eIntranetEventSync:
			TmrDeleteTimer(PollingForwardCancelAlarmHandle);
			PollingForwardCancelAlarmHandle = NULL;

			TimeOutCnt = GetRandDelayTime(eLowpowerIntranetMode);
			PollingForwardCancelAlarmHandle = TmrInsertTimer(GetTmrHd(),
						TMR_PERIOD(TimeOutCnt + TX_DELAY_SHORT_TIME), IntranetPollingCancelAlarm, NULL);
			break;
		
		case Q_eIntranetEventCancelSync:
			TimeOutCnt = GetRandDelayTime(eNormalIntranetMode);
			HalDelayMs(TimeOutCnt);
		
			IntranetSetRadioParamer(&SendMsgParamer, eIntranetRadioMsgSync, NormalIntranetMode, PackNum_4, false);
			IntranetSendRadioMsg(&SendMsgParamer, true);
			HalDelayMs(2);
			IntranetSetRadioParamer(&SendMsgParamer, eIntranetRadioMsgCancleAlarm, NormalIntranetMode, PackNum_4, false);
			IntranetSendRadioMsg(&SendMsgParamer, true);

			Q_TRAN(&SysIdleModeHandleFunc);
			break;


		case Q_eIntranetEventCancelAlarmTimeout:
			TmrDeleteTimer(PollingForwardCancelAlarmHandle);
			PollingForwardCancelAlarmHandle = NULL;

			IntranetSetRadioParamer(&SendMsgParamer, eIntranetRadioMsgSync, NormalIntranetMode, PackNum_1, false);
			IntranetSendRadioMsg(&SendMsgParamer, true);

			IntranetSetRadioParamer(&SendMsgParamer, eIntranetRadioMsgCancleAlarm, NormalIntranetMode, PackNum_2, false);
			IntranetSendRadioMsg(&SendMsgParamer, true);

			IntranetSetRadioParamer(&SendMsgParamer, eIntranetRadioMsgCancleMsgSync, NormalIntranetMode, PackNum_2, false);
			IntranetSendRadioMsg(&SendMsgParamer, true);

			Q_TRAN(&SysIdleModeHandleFunc);
			break;

		default:
			break;
	}
}


/**********************************************************************/
//Description:	IntranetForwardCancelTestModeEvent()										
//Parameters:                        									
//Return:   															
//Date:           
//Author:		quanwu.xu  
/**********************************************************************/
void IntranetForwardCancelTestModeEvent(SystemApiTag  *me,QEvent *e)
{
	uint16_t TimeOutCnt = 0;
	RadioDevTag* pRadioApi = GetRfApiTag();
	IntranetSendMsgParamer SendMsgParamer = {0};
	static TmrHdl PollingCancelTestHandle = NULL;
	static TmrHdl PollingForceEntryLowpowerModeHandle = NULL;

	debug_info("sig=%d\n",e->sig);
	switch (e->sig)
	{
		case Q_ENTRY_SIG:
			SetSysSleepFlag(eSysActiveFlag_CancelMode);
			me->SystemState = eSystemState_Test;

			UartDownLoadHandleFunc(eUartMsgFireAlarm, UartFireAlarmMsgTest, DevTestStop);

			TimeOutCnt = GetRandDelayTime(eLowpowerIntranetMode);

			PollingCancelTestHandle = TmrInsertTimer(GetTmrHd(),
									TMR_PERIOD(TimeOutCnt + TX_DELAY_SHORT_TIME), IntranetPollingCancelTest, NULL);

			PollingForceEntryLowpowerModeHandle = TmrInsertTimer(GetTmrHd(),
											TMR_PERIOD(TimeOut180s), IntranetPollingForcedEntryLowpowerMode, NULL);
			break;

		case Q_EXIT_SIG:
			ResetSysSleepFlag(eSysActiveFlag_CancelMode);
			me->SystemState = eSystemState_Idle;

			TmrDeleteTimer(PollingCancelTestHandle);
			PollingCancelTestHandle = NULL;
			TmrDeleteTimer(PollingForceEntryLowpowerModeHandle);
			PollingForceEntryLowpowerModeHandle = NULL;
			break;

			
		case Q_eIntranetEventEntryLowpowerMode:
			UartDownLoadHandleFunc(eUartMsgFireAlarm, UartFireAlarmMsgTest, DevTestStop);
			Q_TRAN(&SysIdleModeHandleFunc);
			break;

		case Q_eIntranetEventMasterTest:
			Q_TRAN(&IntranetForwardTestModeEvent);
			break;
			
		case Q_eIntranetEventBroadcastTest:
			Q_TRAN(&IntranetBroadcastTestModeEvent);
			break;

		case Q_eIntranetEventSync:
			TmrDeleteTimer(PollingCancelTestHandle);
			PollingCancelTestHandle = NULL;

			TimeOutCnt = GetRandDelayTime(eLowpowerIntranetMode);

			PollingCancelTestHandle = TmrInsertTimer(GetTmrHd(),
				TMR_PERIOD(TimeOutCnt + TX_DELAY_SHORT_TIME), IntranetPollingCancelTest, NULL);
			break;

		case Q_eIntranetEventCancelSync:
			TimeOutCnt = GetRandDelayTime(eNormalIntranetMode);
			HalDelayMs(TimeOutCnt);
		
			IntranetSetRadioParamer(&SendMsgParamer, eIntranetRadioMsgSync, NormalIntranetMode, PackNum_4, false);
			IntranetSendRadioMsg(&SendMsgParamer, true);
			HalDelayMs(2);
			IntranetSetRadioParamer(&SendMsgParamer, eIntranetRadioMsgCancleTest, NormalIntranetMode, PackNum_4, false);
			IntranetSendRadioMsg(&SendMsgParamer, true);

			Q_TRAN(&SysIdleModeHandleFunc);
			break;

		case Q_eIntranetEventCancelTestTimeout:
			IntranetSetRadioParamer(&SendMsgParamer, eIntranetRadioMsgSync, NormalIntranetMode, PackNum_1, false);
			IntranetSendRadioMsg(&SendMsgParamer, true);
			
			HalDelayMs(2);
			
			IntranetSetRadioParamer(&SendMsgParamer, eIntranetRadioMsgCancleTest, NormalIntranetMode, PackNum_4, false);
			IntranetSendRadioMsg(&SendMsgParamer, true);
		
			Q_TRAN(&SysIdleModeHandleFunc);
			break;

		default:
			break;
	}
}

/**********************************************************************/
//Description:	IntranetForwardTestModeEvent()										
//Parameters:                        									
//Return:   															
//Date:           
//Author:		quanwu.xu  
/**********************************************************************/
void IntranetForwardTestModeEvent(SystemApiTag  *me,QEvent *e)
{
	uint16_t TimeOutCnt = 0;
	RadioDevTag* pRadioApi = GetRfApiTag();
	IntranetSendMsgParamer SendMsgParamer = {0};
	static TmrHdl PollingForwardTestHandle = NULL;
	static TmrHdl PollingForceEntryLowpowerModeHandle = NULL;
	static bool Flag = true;

	debug_info("sig=%d\n",e->sig);
	switch (e->sig)
	{
		case Q_ENTRY_SIG:
			SetSysSleepFlag(eSysActiveFlag_TestMode);
			me->SystemState = eSystemState_Test;
		
			Flag = true;
			
			Radio.RadioInit(NormalIntranetMode);

			UartDownLoadHandleFunc(eUartMsgFireAlarm, UartFireAlarmMsgTest, DevTestStart);

			TimeOutCnt = GetRandDelayTime(eLowpowerIntranetMode);
			PollingForwardTestHandle = TmrInsertTimer(GetTmrHd(),
					TMR_PERIOD(TimeOutCnt + TX_DELAY_LONG_TIME), IntranetPollingForwardTest, NULL);

			PollingForceEntryLowpowerModeHandle = TmrInsertTimer(GetTmrHd(),
											TMR_PERIOD(TimeOut180s), IntranetPollingForcedEntryLowpowerMode, NULL);
			break;

		case Q_EXIT_SIG:
			ResetSysSleepFlag(eSysActiveFlag_TestMode);
			me->SystemState = eSystemState_Idle;

			TmrDeleteTimer(PollingForwardTestHandle);
			PollingForwardTestHandle = NULL;
			TmrDeleteTimer(PollingForceEntryLowpowerModeHandle);
			PollingForceEntryLowpowerModeHandle = NULL;
			break;

		case Q_eIntranetEventForwardTestTimeout:
			if(me->SystemState == eSystemState_Test)
			{
				Flag = false;
				TmrDeleteTimer(PollingForwardTestHandle);
				PollingForwardTestHandle = NULL;
				
				IntranetSetRadioParamer(&SendMsgParamer, eIntranetRadioMsgSync, NormalIntranetMode, PackNum_3, false);
				IntranetSendRadioMsg(&SendMsgParamer, true);
				
				IntranetSetRadioParamer(&SendMsgParamer, eIntranetRadioMsgForwardTest, NormalIntranetMode, PackNum_1, false);
				IntranetSendRadioMsg(&SendMsgParamer, true);

				IntranetSetRadioParamer(&SendMsgParamer, eIntranetRadioMsgForwardTest, LowPowerIntranetMode, PackNum_1, false);
				IntranetSendRadioMsg(&SendMsgParamer, true);
				
				IntranetSetRadioParamer(&SendMsgParamer, eIntranetRadioMsgCancleMsgSync, NormalIntranetMode, PackNum_2, false);
				IntranetSendRadioMsg(&SendMsgParamer, true);
			}
			break;

		case Q_eIntranetEventCancelTest:
			Q_TRAN(&IntranetForwardCancelTestModeEvent);
			break;

		case Q_eIntranetEventMasterTest:
			Q_TRAN(&IntranetForwardTestModeEvent);
			break;
			
		case Q_eIntranetEventBroadcastTest:
			Q_TRAN(&IntranetBroadcastTestModeEvent);
			break;

		case Q_eIntranetEventSync:
			if(Flag == true)
			{
				TmrDeleteTimer(PollingForwardTestHandle);
				PollingForwardTestHandle = NULL;

				TimeOutCnt = GetRandDelayTime(eLowpowerIntranetMode);
				PollingForwardTestHandle = TmrInsertTimer(GetTmrHd(),
						TMR_PERIOD(TimeOutCnt + TX_DELAY_LONG_TIME), IntranetPollingForwardTest, NULL);
			}
			break;

		case Q_eIntranetEventEntryLowpowerMode:
			UartDownLoadHandleFunc(eUartMsgFireAlarm, UartFireAlarmMsgTest, DevTestStop);
			Q_TRAN(&SysIdleModeHandleFunc);
			break;

		default:
			break;
	}
}

/**********************************************************************/
//Description:	IntranetRfHandleMsg()											
//Parameters:                        									
//Return:   															
//Date:           
//Author:		quanwu.xu  
/**********************************************************************/
void IntranetPairMsgHandle(IntranetRadioMsgTag *pIntranetMsgTagObj)
{
	eInternatMsgType RadioMsgType =eIntranetRadioMsgNull;
	QEvent IntranetRfHandleEvent = {0};
	RadioDevTag* pRadioApi = GetRfApiTag();

	RadioMsgType = pIntranetMsgTagObj->MsgType;
	
	if((RadioMsgType == eIntranetRadioMsgPairAck) || 
						(RadioMsgType == eIntranetRadioMsgPairNewPairAck) ||
											(RadioMsgType == eIntranetRadioMsgFactoryPairAck))
	{
		if((pIntranetMsgTagObj->DeviceId[0] == 0) && (pIntranetMsgTagObj->DeviceId[1] == 0) &&
				(pIntranetMsgTagObj->DeviceId[2] == 0) && (pIntranetMsgTagObj->DeviceId[3] == 0))
		{
			debug_info("ID is 0\n");
		}
		else
		{
			memcpy((void *)pRadioApi->RadioDevId, (void*)pIntranetMsgTagObj->DeviceId, RfIdLength);
			IntranetRfHandleEvent.sig = Q_eIntranetEventPairAnswer;
			QEP_Post(&IntranetRfHandleEvent);
		}
	}
	else if(RadioMsgType == eIntranetRadioMsgPairNewRequest)
	{
		IntranetRfHandleEvent.sig = Q_eIntranetEventPairRequest;
		QEP_Post(&IntranetRfHandleEvent);
	}
	else if(RadioMsgType == eIntranetRadioMsgPairRequest)
	{
		memcpy((void *)pRadioApi->RadioDevId, (void*)pIntranetMsgTagObj->DeviceId, RfIdLength);
		IntranetRfHandleEvent.sig = Q_eIntranetEventPairOldRequest;
		QEP_Post(&IntranetRfHandleEvent);
	}
}


/**********************************************************************/
//Description:	IntranetRfHandleMsg()											
//Parameters:                        									
//Return:   															
//Date:           
//Author:		quanwu.xu  
/**********************************************************************/
void IntranetRfHandleMsg(uint8_t DataOffset)
{
	eInternatMsgType RadioMsgType =eIntranetRadioMsgNull;
	IntranetRadioMsgTag IntranetMsgTagObj = {0};
	RadioDevTag* pRadioApi = GetRfApiTag();
	QEvent IntranetRfHandleEvent = {0};

	memcpy(&IntranetMsgTagObj, (void *)&RadioReadBuf[DataOffset], INTRANET_PLAYLOAD_LEN);

	RadioMsgType = IntranetMsgTagObj.MsgType;

	if(SystemApiTagObj.SystemState == eSystemState_Pair)
	{
		IntranetPairMsgHandle(&IntranetMsgTagObj);
	}
	else if(CompareRfID(pRadioApi->RadioDevId, IntranetMsgTagObj.DeviceId, RfIdLength))
	{
		printf("Msg=%x\n", RadioMsgType);
		switch(RadioMsgType)
		{
			case eIntranetRadioMsgMasterTest:
				IntranetRfHandleEvent.sig = Q_eIntranetEventMasterTest;
				QEP_Post(&IntranetRfHandleEvent);
				break;
				
			case eIntranetRadioMsgForwardTest:
				IntranetRfHandleEvent.sig = Q_eIntranetEventForwardTest;
				QEP_Post(&IntranetRfHandleEvent);
				break;

			case eIntranetRadioMsgCancleTest:
				IntranetRfHandleEvent.sig = Q_eIntranetEventCancelTest;
				QEP_Post(&IntranetRfHandleEvent);
				break;

			case eIntranetRadioMsgSync:
				IntranetRfHandleEvent.sig = Q_eIntranetEventSync;
				QEP_Post(&IntranetRfHandleEvent);
				break;

			case eIntranetRadioMsgCancleMsgSync:
				IntranetRfHandleEvent.sig = Q_eIntranetEventCancelSync;
				QEP_Post(&IntranetRfHandleEvent);
				break;

			case eIntranetRadioMsgSmokeAlarm:
				IntranetRfHandleEvent.sig = Q_eIntranetEventSmokeAlarm;
				QEP_Post(&IntranetRfHandleEvent);
				break;

			case eIntranetRadioMsgCoAlarm:
				IntranetRfHandleEvent.sig = Q_eIntranetEventCoAlarm;
				QEP_Post(&IntranetRfHandleEvent);
				break;

			case eIntranetRadioMsgCancleAlarm:
				IntranetRfHandleEvent.sig = Q_eIntranetEventCancelAlarm;
				QEP_Post(&IntranetRfHandleEvent);
				break;

			case eIntranetRadioMsgMute:
				IntranetRfHandleEvent.sig = Q_eIntranetEventMute;
				QEP_Post(&IntranetRfHandleEvent);
				break;

			case eIntranetRadioMsgCancleMute:
				IntranetRfHandleEvent.sig = Q_eIntranetEventCancelMute;
				QEP_Post(&IntranetRfHandleEvent);
				break;

#ifdef SUPPORT_SPEAK_LOCATION
			case eIntranetRadioMsgLocationEntryway:
				IntranetRfHandleEvent.sig = Q_eIntranetEventAlarmLocation;
				IntranetRfHandleEvent.Value1 = LocationMsgEntryway;
				QEP_Post(&IntranetRfHandleEvent);
				break;

			case eIntranetRadioMsgLocationBasement:
				IntranetRfHandleEvent.sig = Q_eIntranetEventAlarmLocation;
				IntranetRfHandleEvent.Value1 = LocationMsgBasement;
				QEP_Post(&IntranetRfHandleEvent);
				break;
				
			case eIntranetRadioMsgLocationLivingroom:
				IntranetRfHandleEvent.sig = Q_eIntranetEventAlarmLocation;
				IntranetRfHandleEvent.Value1 = LocationMsgLivingroom;
				QEP_Post(&IntranetRfHandleEvent);
				break;

			case eIntranetRadioMsgLocationDiningroom:
				IntranetRfHandleEvent.sig = Q_eIntranetEventAlarmLocation;
				IntranetRfHandleEvent.Value1 = LocationMsgDiningroom;
				QEP_Post(&IntranetRfHandleEvent);
				break;

			case eIntranetRadioMsgLocationKitchen:
				IntranetRfHandleEvent.sig = Q_eIntranetEventAlarmLocation;
				IntranetRfHandleEvent.Value1 = LocationMsgKitchen;
				QEP_Post(&IntranetRfHandleEvent);
				break;

			case eIntranetRadioMsgLocationHallway:
				IntranetRfHandleEvent.sig = Q_eIntranetEventAlarmLocation;
				IntranetRfHandleEvent.Value1 = LocationMsgHallway;
				QEP_Post(&IntranetRfHandleEvent);
				break;

			case eIntranetRadioMsgLocationMasterbedroom:
				IntranetRfHandleEvent.sig = Q_eIntranetEventAlarmLocation;
				IntranetRfHandleEvent.Value1 = LocationMsgMasterbedroom;
				QEP_Post(&IntranetRfHandleEvent);
				break;

			case eIntranetRadioMsgLocationKidsroom:
				IntranetRfHandleEvent.sig = Q_eIntranetEventAlarmLocation;
				IntranetRfHandleEvent.Value1 = LocationMsgKidsroom;
				QEP_Post(&IntranetRfHandleEvent);
				break;

			case eIntranetRadioMsgLocationGuestroom:
				IntranetRfHandleEvent.sig = Q_eIntranetEventAlarmLocation;
				IntranetRfHandleEvent.Value1 = LocationMsgGuestroom;
				QEP_Post(&IntranetRfHandleEvent);
				break;

			case eIntranetRadioMsgLocationDen:
				IntranetRfHandleEvent.sig = Q_eIntranetEventAlarmLocation;
				IntranetRfHandleEvent.Value1 = LocationMsgDen;
				QEP_Post(&IntranetRfHandleEvent);
				break;

			case eIntranetRadioMsgLocationFamilyroom:
				IntranetRfHandleEvent.sig = Q_eIntranetEventAlarmLocation;
				IntranetRfHandleEvent.Value1 = LocationMsgFamilyroom;
				QEP_Post(&IntranetRfHandleEvent);
				break;

			case eIntranetRadioMsgLocationStudy:
				IntranetRfHandleEvent.sig = Q_eIntranetEventAlarmLocation;
				IntranetRfHandleEvent.Value1 = LocationMsgStudy;
				QEP_Post(&IntranetRfHandleEvent);
				break;

			case eIntranetRadioMsgLocationOffice:
				IntranetRfHandleEvent.sig = Q_eIntranetEventAlarmLocation;
				IntranetRfHandleEvent.Value1 = LocationMsgOffice;
				QEP_Post(&IntranetRfHandleEvent);
				break; 

			case eIntranetRadioMsgLocationWorkshop:
				IntranetRfHandleEvent.sig = Q_eIntranetEventAlarmLocation;
				IntranetRfHandleEvent.Value1 = LocationMsgWorkshop;
				QEP_Post(&IntranetRfHandleEvent);
				break;

			case eIntranetRadioMsgLocationUtilityroom:
				IntranetRfHandleEvent.sig = Q_eIntranetEventAlarmLocation;
				IntranetRfHandleEvent.Value1 = LocationMsgUtilityroom;
				QEP_Post(&IntranetRfHandleEvent);
				break;

			case eIntranetRadioMsgLocationAttic:
				IntranetRfHandleEvent.sig = Q_eIntranetEventAlarmLocation;
				IntranetRfHandleEvent.Value1 = LocationMsgAttic;
				QEP_Post(&IntranetRfHandleEvent);
				break;
#endif

			default:
				break;
		}
	}
	else if(CompareRfID(UniversalID, IntranetMsgTagObj.DeviceId, RfIdLength))
	{
		if(RadioMsgType == eIntranetRadioMsgMasterTest)
		{
			UartDownLoadHandleFunc(eUartMsgFireAlarm, UartFireAlarmMsgTest, DevTestStart);
		}
	}
}
#endif
/**********************************************************************/
//Description:	RFTestHandle()									
//Parameters:                        									
//Return:   															
//Date:           
//Author:		quanwu.xu  
/**********************************************************************/
void RFTestHandle(SystemApiTag  *me,QEvent *e)
{	
	static uint8_t TestStep = 0;

	switch (e->sig)
	{
		case Q_ENTRY_SIG:
			TestStep = 0;
			me->SystemState = eSystemState_RadioTest;
			debug_info("Test OOK\n");
			Radio.RadioInit(OokTestMode);
			break;
			
		case Q_EXIT_SIG:
			break;
			
		case Q_eKeyEventKey1Clik:
			TestStep++;
			switch(TestStep)
			{
				case 1:
					debug_info("Test Fsk\n");
					Radio.RadioInit(FskTestMode);
					break;
				
				case 2:				
					debug_info("Test Rx\n");
					Radio.RadioInit(RxTestMode);
					break;

				case 3:
					SoftwareReset();
					break;

				default:
					break;
			}
			break;

		default:
			break;
	}
}

/**********************************************************************/
//Description:	RfHandleMsg()											
//Parameters:                        									
//Return:   															
//Date:           
//Author:		quanwu.xu  
/**********************************************************************/
void RfHandleMsg(uint8_t DataOffset)
{
	if(Radio.PlayloadLen == HUB_PLAYLOAD_LEN)
	{
		HubRfHandleMsg(DataOffset);
	}
	else if(Radio.PlayloadLen == INTRANET_PLAYLOAD_LEN)
	{
		IntranetRfHandleMsg(DataOffset);
	}
}


/**********************************************************************/
//Description:	RadioService()															  
//Parameters:                        												  
//Return:   																  
//Date:           
//Author:		quanwu.xu  
/**********************************************************************/
void RadioService(void)
{
	uint8_t i = 0;
	uint8_t PlayloadBuf[16] = {0};
	static uint16_t rx_in = 0;
	uint8_t offset = 0;
	uint8_t rx_value_len = 0;			 //数据帧长度

	eRFResult RfRes = Radio.RadioProcess();

	if(Radio.RadioRx(PlayloadBuf, Radio.PlayloadLen) == true)
	{
		for(i=0; i<Radio.PlayloadLen; i++)
		{
			RadioReceiveInput(PlayloadBuf[i]);
		}
		
#ifdef DEBUG_MODE
		debug_printf("Rx:");
		for(i=0; i<Radio.PlayloadLen; i++)
		{
			debug_printf("%x ", PlayloadBuf[i]);
		}		
		debug_printf("\n");
#endif
	}
	else if(RfRes == RF_TX_DONE)
	{
		//Radio.RadioInit(NormalIntranetMode);
		
	}

	while((rx_in < sizeof(RadioReadBuf)) && (Rf_get_queue_total_data() > 0))
	{
		RadioReadBuf[rx_in++] = Rf_Queue_Read_Byte();
	}

	if(rx_in < RfIdLength)
	{
		return;
	}

	//check real  data 
	//printflog("%d,%d\n", rx_in, offset);

	while((rx_in - offset) >= RfIdLength)
	{
		//rx_value_len = wifi_uart_rx_buf[offset + LENGTH_HIGH] * 0x100 + wifi_uart_rx_buf[offset + LENGTH_LOW] + PROTOCOL_HEAD;
		rx_value_len = Radio.PlayloadLen;
		//data not receive over
		if((rx_in - offset) < rx_value_len)
		{
			break;
		}
		
		//数据接收完成,and compare check sum 
		if(GetCrc8((unsigned char *)RadioReadBuf + offset, rx_value_len - 1) == RadioReadBuf[offset + rx_value_len - 1])
		{
			RfHandleMsg(offset);
		}
		else
		{
			//debug_error("Crc Err\n");
		}

	  //offset is right start position
	  //data_handle(offset);
//	  RfHandleMsg(offset);
	  offset += rx_value_len;
	}//end while
	
	rx_in -= offset;
	//copy remain data to first position
	if(rx_in > 0)
	{
		memcpy(RadioReadBuf, RadioReadBuf + offset, rx_in);
	}
}

