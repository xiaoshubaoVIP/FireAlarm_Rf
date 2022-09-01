#include "common.h"

#include "bsp_config.h"
#include "modules_config.h"

#include "app_uart.h"
#include "app_radio.h"


typedef enum _eUartSendState
{
	eUartSendState_Idle = 0x00,
	eUartSendState_Start,
	eUartSendState_Timeout,
	eUartSendState_Success,
	eUartSendState_Fail,
}eUartSendState;

typedef struct _UartSendTag
{
	eUartSendState UartSendState;
	TmrHdl TimerSendUartMsgHandle;
}UartSendTag;

static UartSendTag UartSendTagObject;

UartApi UartApiTag;

#define UartQueueLimit		((uint8_t)96)
#define UartBufLimit		((uint8_t)128)

unsigned char volatile UartQueueBuf[UartQueueLimit] = {0};	//���л���
unsigned char UartReadBuf[UartBufLimit] = {0};				//���ջ���
unsigned char UartSendBuf[UartBufLimit] = {0};				//���ջ���
volatile unsigned char *UartQueueIn = (unsigned char *)UartQueueBuf;
volatile unsigned char *UartQueueOut= (unsigned char *)UartQueueBuf;

void PollingUartRxTimeout(TmrHdl Handle, eTmrCmd eCmd);
void PollingUartSendMsgTimeout(TmrHdl Handle, eTmrCmd eCmd);
void UartHandleMsg(uint8_t DataOffset);

static UartSendMsgParamer GlobalUartSendMsgParamer ={0};
bool UartRxIntflag = false;

#ifdef SUPPORT_SPEAK_LOCATION
extern uint8_t AlarmLocation;
#endif


/**********************************************************************/
//Description:	SerUartRxIntFlag()													  
//Parameters:       
//                        												  
//Return:   
//Date			2019-01-17
//Author:		quanwu.xu
/**********************************************************************/
void SetUartRxIntFlag(void)
{
	UartRxIntflag = true;
}


/**********************************************************************/
//Description:	UartApiTagInit()													  
//Parameters:       
//                        												  
//Return:   
//Date			2019-01-17
//Author:		quanwu.xu
/**********************************************************************/
void UartApiTagInit(void)
{
	HalGpioSetMode(PIN_UART_INT, PIN_MODE_INPUT_PULLUP);
	HalGpioSetIRQMode(PIN_UART_INT, PIN_IRQ_FALLING);
}

/*****************************************************************************
�������� : set_wifi_uart_byte
�������� : дuart�ֽ�
������� : dest:��������ʵ��ַ;
           byte:д���ֽ�ֵ
���ز��� : д����ɺ���ܳ���
*****************************************************************************/
uint8_t SetUartTxBufByte(uint8_t dest, uint8_t byte)
{
	unsigned char *obj = (unsigned char *)UartSendBuf + UART_MSG_DATA_START + dest;

	*obj = byte;
	dest += 1;

	return dest;
}
/*****************************************************************************
�������� : SetUartTxBufBuffer
�������� : дuart_buffer
������� : dest:Ŀ���ַ
           src:Դ��ַ
           len:���ݳ���
���ز��� : ��
*****************************************************************************/
uint8_t SetUartTxBufBuffer(uint8_t dest, uint8_t *src, uint8_t len)
{
	unsigned char *obj = (unsigned char *)UartSendBuf + UART_MSG_DATA_START + dest;

	memcpy(obj,src,len);

	dest += len;
	return dest;
}


/**********************************************************************/
//Description:	UartSendData()															  
//Parameters:                        												  
//Return:   																  
//Date:           
//Author:		quanwu.xu  
/**********************************************************************/
static void UartSendData(uint8_t *SendBuf, uint8_t len)
{
	if((NULL == SendBuf) || (0 == len))
	{
		return;
	}
	
	HalGpioSetMode(PIN_UART_INT, PIN_MODE_OUTPUT);
	HalGpioSet(PIN_UART_INT, PIN_RESET);
	HalDelayMs(2);

	HalUartWrite(UART_NET, SendBuf, len);

	HalGpioSet(PIN_UART_INT, PIN_SET);
	HalGpioSetMode(PIN_UART_INT, PIN_MODE_INPUT_PULLUP);
	HalGpioSetIRQMode(PIN_UART_INT, PIN_IRQ_FALLING);
}


/**********************************************************************/
//Description:	UartWriteFrame()															  
//Parameters:                        												  
//Return:   																  
//Date:           
//Author:		quanwu.xu  
/**********************************************************************/
void UartWriteFrame(uint8_t UartCmd, uint8_t length)
{
	unsigned char check_sum = 0;

	UartSendBuf[UART_MSG_HEAD] = 0xAF;
	UartSendBuf[UART_MSG_TYPE] = UartCmd;
	UartSendBuf[UART_MSG_LENGTH] = length;

	length += UART_MSG_FIXED_LENGTH;
	check_sum = GetCrc8((unsigned char *)UartSendBuf, length - 1);
	UartSendBuf[length - 1] = check_sum;
	UartSendData(UartSendBuf, length);
}


/*****************************************************************************
�������� : UartReceiveInput
�������� : �����ݴ���
������� : value:�����յ��ֽ�����
���ز��� : ��
ʹ��˵�� : ��MCU���ڽ��պ����е��øú���,�������յ���������Ϊ��������
*****************************************************************************/
void UartReceiveInput(unsigned char value)
{
	if((UartQueueIn > UartQueueOut) && ((UartQueueIn - UartQueueOut) >= sizeof(UartQueueBuf)))
	{
		//���ݶ�����
	}
	else if((UartQueueIn < UartQueueOut) && ((UartQueueOut  - UartQueueIn) == 1))
	{
		//���ݶ�����
	}
	else
	{
		//���в���
		if(UartQueueIn >= (unsigned char *)(UartQueueBuf + sizeof(UartQueueBuf)))
		{
			UartQueueIn = (unsigned char *)(UartQueueBuf);
		}

		*UartQueueIn++ = value;
	}
}

/*****************************************************************************
�������� : UartGetQueueTotalData
�������� : ��ȡ����������
������� : ��
���ز��� : ��
*****************************************************************************/
static unsigned char UartGetQueueTotalData(void)
{
	if(UartQueueIn != UartQueueOut)
		return 1;
	else
		return 0;
}
/*****************************************************************************
�������� : Queue_Read_Byte
�������� : ��ȡ����1�ֽ�����
������� : ��
���ز��� : ��
*****************************************************************************/
static unsigned char UartQueueReadByte(void)
{
	unsigned char value = 0;

	if(UartQueueOut != UartQueueIn)
	{
		//������
		if(UartQueueOut >= (unsigned char *)(UartQueueBuf + sizeof(UartQueueBuf)))
		{
			//�����Ѿ���ĩβ
			UartQueueOut = (unsigned char *)(UartQueueBuf);
		}

		value = *UartQueueOut++;   
	}

	return value;
}


/**********************************************************************/
//Description:	UartService()															  
//Parameters:                        												  
//Return:   																  
//Date:           
//Author:		quanwu.xu  
/**********************************************************************/
void UartService(void)
{
	static uint16_t rx_in = 0;
	static TmrHdl PollingUartRxHandle = NULL;

	uint8_t offset = 0;
	uint8_t rx_value_len = 0;			 //����֡����

	uint8_t CharValue = 0;

	if(UartRxIntflag == true)
	{
		PollingUartRxHandle = TmrInsertTimer(GetTmrHd(),
			TMR_PERIOD(TimeOut1s), PollingUartRxTimeout, NULL);
		UartRxIntflag = false;
	}
	else if(UART_INT_is_H())
	{
		if(PollingUartRxHandle != NULL)
		{
			TmrDeleteTimer(PollingUartRxHandle);
			PollingUartRxHandle = NULL;
		}
		ResetSysSleepFlag(eSysActiveFlag_UartInt);
	}

	while((rx_in < sizeof(UartReadBuf)) && (UartGetQueueTotalData() > 0))
	{
		CharValue = UartQueueReadByte();
		UartReadBuf[rx_in++] = CharValue;//UartQueueReadByte();
	}
	
	if(rx_in < UART_MSG_FIXED_LENGTH)
	{
		return;
	}

	//check real  data 
	while((rx_in - offset) > UART_MSG_FIXED_LENGTH)
	{
		if(UartReadBuf[offset + UART_MSG_HEAD] != UART_TX_FRAME_HEADER)
		{
			offset++;	
			continue;
		}

		//rx_value_len = wifi_uart_rx_buf[offset + LENGTH_HIGH] * 0x100 + wifi_uart_rx_buf[offset + LENGTH_LOW] + PROTOCOL_HEAD;
		rx_value_len = UartReadBuf[offset + UART_MSG_LENGTH] + UART_MSG_FIXED_LENGTH;

		if(rx_value_len > UART_MSG_MAX_DATA_LENGTH)
		{
			offset += UART_MSG_LENGTH;
		}

		//data not receive over
		if((rx_in - offset) < rx_value_len)
		{
			break;
		}
		
		//���ݽ������,and compare check sum 
		if(GetCrc8((unsigned char *)UartReadBuf + offset, rx_value_len - 1) == UartReadBuf[offset + rx_value_len - 1])
		{
			UartHandleMsg(offset);
		}
		else
		{
			debug_error("Crc Err\n");
		}

		//offset is right start position
		offset += rx_value_len;
	}//end while
	
	//printflog("%d, %d, %d\n", rx_in, offset, rx_value_len);
	rx_in -= offset;
	//copy remain data to first position
	if(rx_in > 0)
	{
		memcpy(UartReadBuf, UartReadBuf + offset, rx_in);
	}
}


/**********************************************************************/
//Description:	UartSendAck()									
//Parameters:                        									
//Return:   															
//Date:           
//Author:	quanwu.xu  
/**********************************************************************/
bool UartSendMsg(eUartMsgType UartMsgType, uint8_t UartMsgLevel1, uint8_t MsgData)
{
	uint8_t Length = 0;

	GlobalUartSendMsgParamer.MsgType = UartMsgType;
	GlobalUartSendMsgParamer.MsgLevel1 = UartMsgLevel1;
	GlobalUartSendMsgParamer.MsgData = MsgData;
	
	Length = SetUartTxBufByte(Length,UartMsgLevel1);
	if(MsgData != 0)
	{
		Length = SetUartTxBufByte(Length,MsgData);
	}
	UartWriteFrame((uint8_t)UartMsgType, Length);

	if(UartSendTagObject.TimerSendUartMsgHandle == NULL)
	{
		UartSendTagObject.TimerSendUartMsgHandle = TmrInsertTimer(GetTmrHd(),
			TMR_PERIOD(TimeOut1s), PollingUartSendMsgTimeout, NULL);
	}

	return true;
}


/**********************************************************************/
//Description:	UartSendAck()									
//Parameters:                        									
//Return:   															
//Date:           
//Author:	quanwu.xu  
/**********************************************************************/
bool UartSendAck(eUartMsgType UartMsgType, uint8_t TypeAck, uint8_t UartMsgLevel)
{
	uint8_t Length = 0;
	Length = SetUartTxBufByte(Length,TypeAck);
	Length = SetUartTxBufByte(Length,UartMsgLevel);
	UartWriteFrame((uint8_t)UartMsgType, Length);
	
	return true;
}



/**********************************************************************/
//Description:		PollingUartSendMsgTimeout()															  
//Parameters:                        												  
//Return:   																  
//Date:           
//Author:		quanwu.xu  
/**********************************************************************/
void PollingUartSendMsgTimeout(TmrHdl Handle, eTmrCmd eCmd)
{
	static uint8_t ReTxCnt = 0;
	Handle = Handle;	
	switch(eCmd)
	{
		case eInsertTimer:
			ReTxCnt = 0;
			UartSendTagObject.UartSendState = eUartSendState_Start;
			break;
			
		case eTimeOut:
			if(ReTxCnt++>2)
			{
				TmrDeleteTimer(UartSendTagObject.TimerSendUartMsgHandle);
				UartSendTagObject.TimerSendUartMsgHandle = NULL;
				UartSendTagObject.UartSendState = eUartSendState_Fail;
			}
			else
			{
				UartSendMsg(GlobalUartSendMsgParamer.MsgType, GlobalUartSendMsgParamer.MsgLevel1, 
																GlobalUartSendMsgParamer.MsgData);
			}
			break;

		case eDelTimer:
			break;

		default:
			break;
	}	
}


/**********************************************************************/
//Description:		PollingUartRxTimeout()															  
//Parameters:                        												  
//Return:   																  
//Date:           
//Author:		quanwu.xu  
/**********************************************************************/
void PollingUartRxTimeout(TmrHdl Handle, eTmrCmd eCmd)
{
	Handle = Handle;	
	switch(eCmd)
	{
		case eInsertTimer:
			SetSysSleepFlag(eSysActiveFlag_UartInt);
			break;
			
		case eTimeOut:
			ResetSysSleepFlag(eSysActiveFlag_UartInt);
			TmrDeleteTimer(Handle);
			break;

		case eDelTimer:
			break;

		default:
			break;
	}	
}



/**********************************************************************/
//Description:	UartPairMsgHandle()											
//Parameters:                        									
//Return:   															
//Date:           
//Author:	quanwu.xu  
/**********************************************************************/
static void UartPairMsgHandle(uint8_t PairMsg, uint8_t Data1, uint8_t Data2)
{
	QEvent UartEventPair = {0};

	RadioDevTag* pRadioApi = GetRfApiTag();

	if(PairMsg == UartMsgPairAck)
	{
		TmrDeleteTimer(UartSendTagObject.TimerSendUartMsgHandle);
		UartSendTagObject.TimerSendUartMsgHandle = NULL;
		if(Data1 == UartMsgPairSuccess)
		{
			UartEventPair.sig = Q_eSystemEventPairFinished;
			QEP_Post(&UartEventPair);
		}
	}
	else
	{
		UartSendAck(eUartMsgPair, UartMsgPairAck, PairMsg);

		switch (PairMsg)
		{
			case UartMsgPairHubMode:
				UartEventPair.sig = Q_eSystemEventPairToHubMode;
				QEP_Post(&UartEventPair);
				pRadioApi->DevType = (eRfDeviceType)Data1;
				pRadioApi->ProductType = (eProductType)Data2;
				debug_printf("Dev=%d, Pro=%d Hub\n", Data1, Data2);
				break;

#ifdef SUPPORT_INTRANET_FUNCTION
			case UartMsgPairFactoryMode:
				UartEventPair.sig = Q_eIntranetEventPairFactoryMode;
				QEP_Post(&UartEventPair);
				break;

			case UartMsgPairSlaverMode:
				UartEventPair.sig = Q_eIntranetEventPairIntranetSlaverMode;
				QEP_Post(&UartEventPair);
				pRadioApi->DevType = (eRfDeviceType)Data1;
				pRadioApi->ProductType = (eProductType)Data2;
				debug_printf("Dev=%d, Pro=%d Slaver\n", Data1, Data2);
				break;

			case UartMsgPairMasterMode:
				UartEventPair.sig = Q_eIntranetEventPairIntranetMasterMode;
				QEP_Post(&UartEventPair);
				pRadioApi->DevType = (eRfDeviceType)Data1;
				pRadioApi->ProductType = (eProductType)Data2;
				debug_printf("Dev=%d, Pro=%d Master\n", Data1, Data2);
				break;

			case UartMsgPairResetId:
				if(SaveIDProcess(pRadioApi->RadioDevId, false, ePairedResetId) == true)
				{
					SoftwareReset();
				}
				break;
#endif
			case UartMsgPairStop:
				UartEventPair.sig = Q_eIntranetEventPairStop;
				QEP_Post(&UartEventPair);
				break;

			default:
				break;
		}
	}	
}


/**********************************************************************/
//Description:	UartFireAlarmMsgHandle()															  
//Parameters:                        												  
//Return:   																  
//Date:           
//Author:		quanwu.xu  
/**********************************************************************/
void UartFireAlarmMsgHandle(uint8_t FireAlarmMsg, uint8_t Data1, uint8_t Data2)
{
	QEvent UartEventUpdate = {0};
	RadioDevTag* pRadioApi = GetRfApiTag();

	if(FireAlarmMsg == UartFireAlarmMsgAck)
	{
		TmrDeleteTimer(UartSendTagObject.TimerSendUartMsgHandle);
		UartSendTagObject.TimerSendUartMsgHandle = NULL;
		UartSendTagObject.UartSendState = eUartSendState_Success;
	}
	else
	{
		UartSendAck(eUartMsgFireAlarm, UartFireAlarmMsgAck, FireAlarmMsg);
		if((GetRfPairedMode() == ePairedHub) || (GetRfPairedMode() == ePairedIntranet))
		{
			debug_printf("FireAlarm: %d, %d, %d\n", FireAlarmMsg, Data1, Data2);
			switch (FireAlarmMsg)
			{
				case UartFireAlarmMsgMute:
					if(GetRfPairedMode() == ePairedHub)
					{
						UartEventUpdate.sig = Q_eUartEventUpdateMute;
						UartEventUpdate.Value1 = FirmAlarmMsgMute;
					}
					else
					{
						UartEventUpdate.sig = Q_eIntranetEventBroadcastCancelAlarm;
						UartEventUpdate.Value1 = FirmAlarmMsgCancelAlarm;
					}
					UartEventUpdate.Value2 = Data1;
					UartEventUpdate.Value3 = Data2;
					QEP_Post(&UartEventUpdate);
					break;

				case UartFireAlarmMsgCancelMute:
					UartEventUpdate.sig = Q_eUartEventUpdateCancelMute;
					UartEventUpdate.Value1 = FirmAlarmMsgCancelMute;
					UartEventUpdate.Value2 = Data1;
					UartEventUpdate.Value3 = Data2;
					QEP_Post(&UartEventUpdate);
					break;

				case UartFireAlarmMsgTest:
					if(GetRfPairedMode() == ePairedHub)
					{
						UartEventUpdate.sig = Q_eUartEventUpdateTest;
					}
					else
					{
						if(DevTestStop == Data1)
						{
							UartEventUpdate.sig = Q_eIntranetEventBroadcastCancelTest;
						}
						else if(DevTestStart == Data1)
						{
							UartEventUpdate.sig = Q_eIntranetEventBroadcastTest;
						}
					}

					UartEventUpdate.Value1 = FirmAlarmMsgTest;
					UartEventUpdate.Value2 = Data1;
					UartEventUpdate.Value3 = Data2;
					QEP_Post(&UartEventUpdate);
					break;

				case UartFireAlarmMsgFault:
					if(GetRfPairedMode() == ePairedHub)
					{
						UartEventUpdate.sig = Q_eUartEventUpdateDevFault;
						UartEventUpdate.Value1 = FirmAlarmMsgFault;
						UartEventUpdate.Value2 = Data1;
						UartEventUpdate.Value3 = Data2;
						QEP_Post(&UartEventUpdate);
					}
					break;

				case UartFireAlarmMsgLowBat:
					if(GetRfPairedMode() == ePairedHub)
					{
						UartEventUpdate.sig = Q_eUartEventUpdateBat;
						UartEventUpdate.Value1 = FirmAlarmMsgLowBat;
						UartEventUpdate.Value2 = Data1;
						UartEventUpdate.Value3 = Data2;
						QEP_Post(&UartEventUpdate);
					}
					break;

				case UartFireAlarmMsgSmokeAlarm:
					if(GetRfPairedMode() == ePairedHub)
					{
						UartEventUpdate.sig = Q_eUartEventUpdateAlarm;
					}
					else
					{
						UartEventUpdate.sig = Q_eIntranetEventBroadcastSmokeAlarm;
#ifdef SUPPORT_SPEAK_LOCATION
						AlarmLocation = (Data1 + 0xCA);
#endif
					}
					UartEventUpdate.Value1 = FirmAlarmMsgSmokeAlarm;
					UartEventUpdate.Value2 = Data1;
					UartEventUpdate.Value3 = Data2;
					QEP_Post(&UartEventUpdate);
					break;

				case UartFireAlarmMsgCoAlarm:
					if(GetRfPairedMode() == ePairedHub)
					{
						UartEventUpdate.sig = Q_eUartEventUpdateAlarm;
					}
					else
					{
						UartEventUpdate.sig = Q_eIntranetEventBroadcastCoAlarm;
#ifdef SUPPORT_SPEAK_LOCATION
						AlarmLocation = (Data1 + 0xCA);
#endif
					}
					UartEventUpdate.Value1 = FirmAlarmMsgCoAlarm;
					UartEventUpdate.Value2 = Data1;
					UartEventUpdate.Value3 = Data2;
					QEP_Post(&UartEventUpdate);
					break;
		
				case UartFireAlarmMsgHeatAlarm:
					if(GetRfPairedMode() == ePairedHub)
					{
						UartEventUpdate.sig = Q_eUartEventUpdateAlarm;
					}
					else
					{
						UartEventUpdate.sig = Q_eIntranetEventBroadcastHeatAlarm;
					}
					UartEventUpdate.Value1 = FirmAlarmMsgHeatAlarm;
					UartEventUpdate.Value2 = Data1;
					UartEventUpdate.Value3 = Data2;
					QEP_Post(&UartEventUpdate);
					break;
					
				case UartFireAlarmMsgGasAlarm:
					if(GetRfPairedMode() == ePairedHub)
					{
						UartEventUpdate.sig = Q_eUartEventUpdateAlarm;
					}
					else
					{
						UartEventUpdate.sig = Q_eIntranetEventBroadcastGasAlarm;
					}
					UartEventUpdate.Value1 = FirmAlarmMsgGasAlarm;
					UartEventUpdate.Value2 = Data1;
					UartEventUpdate.Value3 = Data2;
					QEP_Post(&UartEventUpdate);
					break;

				case UartFireAlarmMsgCancelAlarm:
					if(GetRfPairedMode() == ePairedHub)
					{
						UartEventUpdate.sig = Q_eUartEventUpdateCancelAlarm;
					}
					else
					{
						UartEventUpdate.sig = Q_eIntranetEventBroadcastCancelAlarm;
					}
					UartEventUpdate.Value1 = FirmAlarmMsgCancelAlarm;
					UartEventUpdate.Value2 = Data1;
					UartEventUpdate.Value3 = Data2;
					QEP_Post(&UartEventUpdate);
					break;

				case UartFireAlarmMsgUpdateCoPpm:
					if(GetRfPairedMode() == ePairedHub)
					{
						UartEventUpdate.sig = Q_eUartEventUpdateCoPpm;
						UartEventUpdate.Value1 = FirmAlarmMsgUpdateCoPpm;
						UartEventUpdate.Value2 = Data1;
						UartEventUpdate.Value3 = Data2;
						QEP_Post(&UartEventUpdate);
					}
					break;

				default:
					break;
			}
		}
	}
}


/**********************************************************************/
//Description:	UartHandleMsg()											
//Parameters:                        									
//Return:   															
//Date:           
//Author:	quanwu.xu  
/**********************************************************************/
void UartHandleMsg(uint8_t DataOffset)
{
	uint8_t i, PrintLen;
	uint8_t DataBuf[4] = {0};
	eUartMsgType UartMsgType = (eUartMsgType)UartReadBuf[DataOffset + UART_MSG_TYPE];
	uint8_t UartMsgDataLength = UartReadBuf[DataOffset + UART_MSG_LENGTH];
	RadioDevTag* pRadioApi = GetRfApiTag();

	memcpy((void *)DataBuf, (void *)&UartReadBuf[DataOffset + UART_MSG_DATA_START], UartMsgDataLength);

	PrintLen = UartMsgDataLength + UART_MSG_FIXED_LENGTH;

	if(UartMsgType == eUartMsgPair)
	{
		UartPairMsgHandle(DataBuf[0], DataBuf[1], DataBuf[2]);
	}
	else
	{
		switch(UartMsgType)
		{
			case eUartMsgFireAlarm:
				UartFireAlarmMsgHandle(DataBuf[0], DataBuf[1], DataBuf[2]);
				break;
				
			default:
				break;
		}
	}

	debug_printf("Uart Rx: ");
	for(i=0; i<PrintLen; i++)
	{
		debug_printf("[%x]", UartReadBuf[DataOffset + i]);	
	}
	debug_printf("\n");	
	
}

/**********************************************************************/
//Description:	UartDownLoadHandleFunc()								
//Parameters:                        									
//Return:   															
//Date:           
//Author:	quanwu.xu  
/**********************************************************************/
void UartDownLoadHandleFunc(eUartMsgType MsgType, uint8_t Value1, uint8_t Value2)
{
	debug_printf("%s,in, %d\n", __FUNCTION__, __LINE__);
	
	UartSendMsg(MsgType, Value1, Value2);
	
	while(UartSendTagObject.TimerSendUartMsgHandle != NULL)
	{
		SystemPolling();
		RadioService();
		UartService();

		if((UartSendTagObject.UartSendState == eUartSendState_Success)
				|| (UartSendTagObject.UartSendState == eUartSendState_Fail))
		{
			break;
		}
	}
}


