#ifndef __SYSTEM_H__
#define __SYSTEM_H__

#include <stdint.h>
#include <stdlib.h>
#include "qep_port.h"
#include "base_types.h"
#include "polling.h"

#define SystemPostFIFI QEP_Post

#if 0
#define GET_FLASH_MEMBER_ADDR(me) (uint16_t)(&(((FlashRadioDataMap*)0)->me))
#define FLASH_DATA_EEPROM_START_PHYSICAL_ADDRESS	((uint32_t)0x00007E00u) /*!< Data flash: start address */

extern void hal_flash_write(unsigned long addr, uint8_t* buf, unsigned short len);
extern void hal_flash_read(unsigned long addr, uint8_t* buf, unsigned short len);

#define FlashGetData(name, buf) do { \
	hal_flash_read(GET_FLASH_MEMBER_ADDR(name) + FLASH_DATA_EEPROM_START_PHYSICAL_ADDRESS, (uint8_t*)buf, sizeof(buf)); \
}while(0)

#define FlashSetData(name, buf) do { \
	hal_flash_write(GET_FLASH_MEMBER_ADDR(name) + FLASH_DATA_EEPROM_START_PHYSICAL_ADDRESS, (uint8_t*)buf, sizeof(buf)); \
}while(0)

#pragma pack(1)
typedef struct {
	uint8_t DevId[4];
	uint8_t Udid[4];
	uint8_t DevType;
	uint8_t FatherDevType;
	uint8_t ProductType;
	uint8_t SelfDevNum;
	uint8_t FatherDevNum;
	uint8_t PairMode;
}FlashRadioDataMap;
#pragma pack()
#endif


//Rand Delay Time mode
typedef enum _eDelayMode{
	eNullTimeMode = 0,
	eNormalTimeMode,
	eLowpowerTimeMode,
	eNormalIntranetMode,
	eLowpowerIntranetMode,
}eDelayMode;

#define POLLING_SEND_MSG_INTERVAL_TIME		50
#define DELAY_SEND_MSG_INTERVAL_TIME		100
#define TX_DELAY_SHORT_TIME					25
#define TX_DELAY_LONG_TIME					5200

typedef enum _eSysActiveFlag
{
	eSysActiveFlag_NULL = (uint32_t)0x00,
	eSysActiveFlag_KeyPress = (uint32_t)0x01<<0,
	eSysActiveFlag_KeyWait = (uint32_t)0x01<<1,
	eSysActiveFlag_UartInt = (uint32_t)0x01<<2,
	eSysActiveFlag_RadioInt = (uint32_t)0x01<<3,
	eSysActiveFlag_UpdateTest = (uint32_t)0x01<<4,
	eSysActiveFlag_UpdateAlarm = (uint32_t)0x01<<5,
	eSysActiveFlag_UpdateCancelAlarm = (uint32_t)0x01<<6,
	eSysActiveFlag_UpdateMute = (uint32_t)0x01<<7,
	eSysActiveFlag_UpdateCancelMute = (uint32_t)0x01<<8,
	eSysActiveFlag_UpdateHearbeat = (uint32_t)0x01<<9,
	eSysActiveFlag_UpdateBatLevel = (uint32_t)0x01<<10,
	eSysActiveFlag_UpdateDevErr = (uint32_t)0x01<<11,
	eSysActiveFlag_UpdateDeleteRes = (uint32_t)0x01<<12,
	
	eSysActiveFlag_SearchHubNode = (uint32_t)0x01<<13,
	eSysActiveFlag_SearchRepeaterNode = (uint32_t)0x01<<14,
	
	eSysActiveFlag_DownLoadTest = (uint32_t)0x01<<15,
	eSysActiveFlag_DownLoadMute = (uint32_t)0x01<<16,
	eSysActiveFlag_DownLoadCancelMute = (uint32_t)0x01<<17,
	eSysActiveFlag_DownLoadAlarm = (uint32_t)0x01<<18,
	eSysActiveFlag_DownLoadCancelAlarm = (uint32_t)0x01<<19,

	eSysActiveFlag_Wakeup = (uint32_t)0x01<<20,
	eSysActiveFlag_Pair = (uint32_t)0x01<<21,
	eSysActiveFlag_FactoryPairMode = (uint32_t)0x01<<22,
	eSysActiveFlag_BoardcastMode = (uint32_t)0x01<<23,
	eSysActiveFlag_ForwardMode = (uint32_t)0x01<<24,
	eSysActiveFlag_AlarmMode = (uint32_t)0x01<<25,
	eSysActiveFlag_TestMode = (uint32_t)0x01<<26,
	eSysActiveFlag_CancelMode = (uint32_t)0x01<<27,

	eSysActiveFlag_All = (uint32_t)0x7FFFFFFF,
}eSysActiveFlag;


typedef enum
{
	//SystemEvent
	Q_eSystemEventIdle = Q_USER_SIG,

	//SysemWakeupTimeout
	Q_eSystemEventLowpowerMode,					//5
	Q_eSystemEventWakeupTimeout,

	//Hearbeat
	Q_eSystemEventUpdateHeartbeat,

	//PairEvent
	Q_eSystemEventPairToHubMode,
	Q_eSystemEventPairStart,
	Q_eSystemEventPairSuccess,					//10
	Q_eSystemEventPairFinished,
	Q_eSystemEventPairFail,

	//KeyEvent
	Q_eKeyEventKey1Clik,
	Q_eKeyEventKey1LongClik,
	Q_eKeyEventKey2Clik,						//15
	Q_eKeyEventKey2LongClik,

	//InitEvent
	Q_eInitEventUpdateCancelAlarm,

	//UartEvent
	Q_eUartEventUpdateTest,
	Q_eUartEventUpdateAlarm,
	Q_eUartEventUpdateCancelAlarm,				//20
	Q_eUartEventUpdateMute,
	Q_eUartEventUpdateCancelMute,
	Q_eUartEventUpdateBat,
	Q_eUartEventUpdateDevFault,
	Q_eUartEventUpdateCoPpm,					//25

	//RadioSendMsgEvent
	Q_eRadioEventSendMsgStart,
	Q_eRadioEventSendMsgTimeout,
	Q_eRadioEventBroadcastMsgTimeout,
	Q_eRadioEventBroadcastCancelMsgTimeout,
	Q_eRadioEventRandForwardMsgTimeout,			//30
	Q_eRadioEventRandForwardCancelMsgTimeout,	
	Q_eRadioEventSendMsgFail,
	Q_eRadioEventSendMsgOk,

	//UartSendMsgEvent
	Q_eUartEventSendMsgStart,
	Q_eUartEventSendMsgTimeout,					//35
	Q_eUartEventSendMsgFail,
	Q_eUartEventSendMsgOk,

	//SearchEvent
	Q_eRadioSearchFatherNodeStart,
	Q_eRadioSearchHubNodeSuccess,
	Q_eRadioSearchRepeaterNodeSuccess,			//40
	Q_eRadioSearchNodeFail,
	Q_eRadioSearchFatherNodeSuccessThenRetransmit,
	Q_eRadioSearchFatherNodeSuccessThenRetransmitFinished,

	//RadioEvent
	Q_eRadioEventDownloadWakeup,
	Q_eRadioEventDownloadDelete,				//45
	Q_eRadioEventDownloadTest,
	Q_eRadioEventDownloadAlarm,
	Q_eRadioEventDownloadCancelAlarm,
	Q_eRadioEventDownloadMute,
	Q_eRadioEventDownloadCancelMute,			//50

	//AlarmEvent
	Q_eSystemEventInitialAlarmModeIn,
	Q_eSystemEventAlarmModeIn,


	//SysemWakeupTimeout
	Q_eIntranetEventEntryLowpowerMode,
	Q_eIntranetEventWakeupTimeout,

	Q_eIntranetEventPairFactoryMode,			//55
	Q_eIntranetEventPairIntranetSlaverMode,
	Q_eIntranetEventPairIntranetMasterMode,
	Q_eIntranetEventPairResetId,
	Q_eIntranetEventPairOldRequest,
	Q_eIntranetEventPairRequest,				//60
	Q_eIntranetEventPairAnswer,
	Q_eIntranetEventPairSuccess,
	Q_eIntranetEventPairFail,
	Q_eIntranetEventPairStop,

	//UartEvent
	Q_eIntranetEventBroadcastTest,				//65
	Q_eIntranetEventBroadcastCancelTest,
	Q_eIntranetEventBroadcastSmokeAlarm,
	Q_eIntranetEventBroadcastCoAlarm,
	Q_eIntranetEventBroadcastHeatAlarm,
	Q_eIntranetEventBroadcastGasAlarm,			//70
	Q_eIntranetEventBroadcastCancelAlarm,
	Q_eIntranetEventBroadcastMute,
	Q_eIntranetEventBroadcastCancelMute,

	//Sync Event
	Q_eIntranetEventSync,
	Q_eIntranetEventCancelSync,					//75

	//RadioEvent
	Q_eIntranetEventMasterTest,
	Q_eIntranetEventForwardTest,
	Q_eIntranetEventCancelTest,
	Q_eIntranetEventSmokeAlarm,
	Q_eIntranetEventCoAlarm,					//80
	Q_eIntranetEventCancelAlarm,
	Q_eIntranetEventMute,
	Q_eIntranetEventCancelMute,
#ifdef SUPPORT_SPEAK_LOCATION
	Q_eIntranetEventAlarmLocation,
#endif

	//Polling Event
	Q_eIntranetEventStartEntryTestTimeout,
	Q_eIntranetEventForwardTestTimeout,
	Q_eIntranetEventCancelTestTimeout,
	Q_eIntranetEventStartEntryAlarmTimeout,
	Q_eIntranetEventForwardAlarmTimeout,
	Q_eIntranetEventCancelAlarmTimeout,



	Q_eSystemEventMax,
}eSystemEvent;

typedef QFsm SystemTag;

typedef enum _eSystemState
{
	eSystemState_Idle = 0x00,
	eSystemState_RadioTest,
	eSystemState_Pair,
	eSystemState_Test,
	eSystemState_CancelTest,
	eSystemState_Alarm,
	eSystemState_Mute,
	eSystemState_CancelMute,
	eSystemState_CancelAlarm,
}eSystemState;

typedef struct __SystemApiTag
{
	QFsm  super_;
	
	bool PowerOnFlag;
	bool ActiveAlarmFlag;
	eSystemState SystemState;
	uint32_t SystemTick;
	uint32_t SysActiveFlag;
}SystemApiTag;

extern SystemApiTag SystemApiTagObj;
void SetSysSleepFlag(eSysActiveFlag Flag);
uint32_t GetSysSleepFlag(void);
void ResetSysSleepFlag(eSysActiveFlag Flag);
uint8_t GetCrc8(uint8_t *buf, uint8_t len);
eSystemState  GetSystemState(void);
void SysTickAdd(void);
uint32_t SysGetTick(void);
uint16_t GetRandDelayTime(eDelayMode DelayMode);

#endif
