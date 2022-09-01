#ifndef	_APP_RADIO_H_
#define _APP_RADIO_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "common.h"

#define RfIdLength				((uint8_t)4)

#ifdef SUPPORT_HC32L100
#define FLASH_ADDR_START						((uint32_t)(0x00007E00u))
#define FLASH_ADDR_RF_ID						((uint32_t)(FLASH_ADDR_START))
#define FLASH_ADDR_PAIR_FLAG					((uint32_t)(FLASH_ADDR_RF_ID + 4))
#define FLASH_ADDR_DEV_NUM						((uint32_t)(FLASH_ADDR_PAIR_FLAG + 4))
#define FLASH_ADDR_DEV_TYPE						((uint32_t)(FLASH_ADDR_DEV_NUM + 4))
#define FLASH_ADDR_PRO_TYPE						((uint32_t)(FLASH_ADDR_DEV_TYPE + 4))
#define FLASH_ADDR_FATHER_NUM					((uint32_t)(FLASH_ADDR_PRO_TYPE + 4))
#define FLASH_ADDR_UDID_NUM						((uint32_t)0x00007FF4)
#elif defined(SUPPORT_BAT32G133)
#define FLASH_ADDR_START						((uint32_t)(0x00500100u))
#define FLASH_ADDR_UDID_NUM						((uint32_t)FLASH_ADDR_START)
#define FLASH_ADDR_RF_ID						((uint32_t)(FLASH_ADDR_START + 4))
#define FLASH_ADDR_PAIR_FLAG					((uint32_t)(FLASH_ADDR_RF_ID + 4))
#define FLASH_ADDR_DEV_NUM						((uint32_t)(FLASH_ADDR_PAIR_FLAG + 4))
#define FLASH_ADDR_DEV_TYPE						((uint32_t)(FLASH_ADDR_DEV_NUM + 4))
#define FLASH_ADDR_PRO_TYPE						((uint32_t)(FLASH_ADDR_DEV_TYPE + 4))
#define FLASH_ADDR_FATHER_NUM					((uint32_t)(FLASH_ADDR_PRO_TYPE + 4))

#endif


#define	Rf_Int_Port			(0x02)
#define	Rf_Int_Pin			(0x03)

#define PackNum_1 	((uint8_t)0x00)
#define PackNum_2 	((uint8_t)0x01)
#define PackNum_3 	((uint8_t)0x02)
#define PackNum_4	((uint8_t)0x03)
#define PackNum_8	((uint8_t)0x07)
#define PackNum_16  ((uint8_t)0x0F)
#define PackNum_32  ((uint8_t)0x1F)

//默认设备号
#ifdef SUPPORT_TYPE_HUB
#define DEFAULT_DEV_NUM	((uint8_t)0x0)
#else
#define DEFAULT_DEV_NUM	((uint8_t)0xFF)
#endif

//消息属性
//(应用GateWay 协议)
#define RfMsgPropertyNull					((uint8_t)0)
#define RfMsgPropertyNeedAck				((uint8_t)0x01)
#define RfMsgPropertyNeedUpTransfer			((uint8_t)0x02)
#define RfMsgPropertyNeedDownTransfer		((uint8_t)0x04)
#define RfMsgPropertyIsUpTransfer			((uint8_t)0x08)
#define RfMsgPropertyIsDownTransfer			((uint8_t)0x10)

//特定目标设备号
#define  HubNumber						((uint8_t)0)		//Hub
#define  TxToAllDevices 				((uint8_t)0xFF)		//all Devices
#define  TxToAllRepeaters				((uint8_t)0xFE)		//all Repeater
#define  TxToAllFireAlarms				((uint8_t)0xFD)		//all FireAlarm


//设备类型
typedef enum _eRfDeviceType{
	eTypeNull = 0x00,
	eTypeHub,                    //Hub设备
	eTypeRepeater,               //中继器
	eTypeSmoke,                  //烟感报警器
	eTypeCo,                     //Co报警器
	eTypeHeat,					 //热报警器
	eTypeGas,					 //燃气
	eTypeSmokeAndCo,             //SC复合型报警器
	eTypeHeatAndCo,              //HC复合型报警器
	eTypeGasAndCo,               //GC复合报警器
	eTypeWaterSensor,            //水浸 
	eTypePlug,                   //插座
	eTypeRomote,                 //遥控器
	eTypePir,                    //Pir
	eTypeMagnet,                 //门窗磁
	eTypeDriveWay,               //车道报警器
	eTypeGlassBrokenAlarm,       //玻璃碎探测报警器
	eTypeSos,                    //求救器
	eTypeActuator,               //执行器
	eTypeKeyBoard,				 //键盘设备
	eTypeDoorBell,				 //门铃
	eTypeHorn,					 //喇叭
	eTypeCamera,				 //摄像头
	eTypeWeatherStation,		 //温湿计
}eRfDeviceType;


//一级消息类型
typedef enum _eHubMsgType{
	eRfMsgNull = 0x00,
	eRfMsgWakeup, 
	eRfMsgTraverse,
	eRfMsgPair,
	eRfMsgSync,
	eRfMsgDeleteDev,
	eRfMsgSearchDev,
	eRfMsgHeartBeat,
	eRfMsgFireAlarm,		//0x08
	eRfMsgWaterSensor,
	eRfMsgPlug,
	eRfMsgRomote,
	eRfMsgPir,
	eRfMsgMagnet,
	eRfMsgDriveWay,
	eRfMsgGlassBrokenAlarm,
	eRfMsgSos,
	eRfMsgActuator,
	eRfMsgKeyBoard,
	eRfMsgDoorBell,
	eRfMsgHorn,
	eRfMsgCamera,
	eRfMsgWeatherStation,

	eRfMsgFactoyTest = 0xFF,
}eHubMsgType;

typedef enum _ePairedMode
{
	ePairedNull = 0,
	ePairedHub = 0xA1,
	ePairedIntranet = 0xC2,
	ePairedResetId = 0xB3,
}ePairedMode;

//命令统称
#define DevLevel2MsgAck		((uint8_t)0x01)
#define DevLevel2MsgTest	((uint8_t)0x04)
#define DevLevel2MsgFault	((uint8_t)0x05)
#define DevLevel2MsgLowBat	((uint8_t)0x06)

//二级消息类型（唤醒）
#define WakeupMsgAck		((uint8_t)0x01)
#define WakeupMsgAllDev		((uint8_t)0x02)
#define WakeupMsgSingleDev	((uint8_t)0x03)

//二级消息类型（配对）
#define PairMsgAck					((uint8_t)0x01)
#define PairMsgRequest				((uint8_t)0x02)
#define PairMsgStart				((uint8_t)0x03)
#define PairMsgStop					((uint8_t)0x04)
#define PairMsgSuccess				((uint8_t)0x05)
#define PairMsgFail					((uint8_t)0x06)
#define PairMsgFactoryRequest		((uint8_t)0x07)
#define PairMsgFactorySuccessAck	((uint8_t)0x08)

//二级消息类型（同步）
#define SyncMsgAck								((uint8_t)0x01)
#define SyncMsgNoticeStartBroadcast				((uint8_t)0x02)
#define SyncMsgNoticeStopBroadcast				((uint8_t)0x03)
#define SyncMsgNoticeStartForward				((uint8_t)0x04)
#define SyncMsgNoticeStoptForward				((uint8_t)0x05)
#define SyncMsgNoticeStartCancel				((uint8_t)0x06)
#define SyncMsgNoticeStopCancel					((uint8_t)0x07)

//二级消息类型（删除）
#define DeleteDevMsgAck		((uint8_t)0x01)
#define DeleteDevMsgNotice 	((uint8_t)0x02)
#define DeleteDevMsgSuccess ((uint8_t)0x03)
#define DeleteDevMsgFail 	((uint8_t)0x04
 //二级消息类型（Search）
#define SearchMsgAck			((uint8_t)0x01)
#define SearchMsgAllSameTypeDev	((uint8_t)0x02)
#define SearchMsgSingleDev		((uint8_t)0x03)
#define SearchMsgSuccess		((uint8_t)0x04)
#define SearchMsgFail			((uint8_t)0x05)
#define SearchMsgStopSend		((uint8_t)0x06)

//二级消息类型（Hearbeat）
#define HearbeatMsgAck		((uint8_t)0x01)
#define HearbeatMsgUpdate	((uint8_t)0x02)

//二级消息类型（消防报警器）
#define FirmAlarmMsgAck				((uint8_t)0x01)
#define FirmAlarmMsgMute			((uint8_t)0x02)
#define FirmAlarmMsgCancelMute		((uint8_t)0x03)
#define FirmAlarmMsgTest			((uint8_t)0x04)
#define FirmAlarmMsgFault			((uint8_t)0x05)
#define FirmAlarmMsgLowBat			((uint8_t)0x06)
#define FirmAlarmMsgSmokeAlarm		((uint8_t)0x07)
#define FirmAlarmMsgCoAlarm			((uint8_t)0x08)
#define FirmAlarmMsgHeatAlarm		((uint8_t)0x09)
#define FirmAlarmMsgGasAlarm		((uint8_t)0x0A)
#define FirmAlarmMsgCancelAlarm		((uint8_t)0x0B)
#define FirmAlarmMsgUpdateCoPpm		((uint8_t)0x0C)

//二级消息类型（WaterSensor）
#define WaterSensorMsgAck			((uint8_t)0x01)
#define WaterSensorMsgMute			((uint8_t)0x02)
#define WaterSensorMsgCancelMute	((uint8_t)0x03)
#define WaterSensorMsgTest			((uint8_t)0x04)
#define WaterSensorMsgFault			((uint8_t)0x05)
#define WaterSensorMsgLowBat		((uint8_t)0x06)
#define WaterSensorMsgAlarm			((uint8_t)0x07)
#define WaterSensorMsgCancelAlarm	((uint8_t)0x08)

//二级消息类型（Plug）
#define PlugMsgAck					((uint8_t)0x01)
#define PlugMsgMute					((uint8_t)0x02)
#define PlugMsgCancelMute			((uint8_t)0x03)
#define PlugMsgTest					((uint8_t)0x04)
#define PlugMsgFault				((uint8_t)0x05)
#define PlugMsgLowBat				((uint8_t)0x06)
#define PlugMsgTurnOn				((uint8_t)0x07)
#define PlugMsgTurnOff				((uint8_t)0x08)

//二级消息类型（Romote）
#define RomoteMsgAck				((uint8_t)0x01)
#define RomoteMsgMute				((uint8_t)0x02)
#define RomoteMsgCancelMute			((uint8_t)0x03)
#define RomoteMsgTest				((uint8_t)0x04)
#define RomoteMsgFault				((uint8_t)0x05)
#define RomoteMsgLowBat				((uint8_t)0x06)
#define RomoteMsg_Key1				((uint8_t)0x07)
#define RomoteMsg_Key2				((uint8_t)0x08)
#define RomoteMsg_Key3				((uint8_t)0x09)
#define RomoteMsg_Key4				((uint8_t)0x0A)

//二级消息类型（Magnet）
#define MagnetMsgAck				((uint8_t)0x01)
#define MagnetMsgMute				((uint8_t)0x02)
#define MagnetMsgCancelMute			((uint8_t)0x03)
#define MagnetMsgTest				((uint8_t)0x04)
#define MagnetMsgFault				((uint8_t)0x05)
#define MagnetMsgLowBat				((uint8_t)0x06)
#define MagnetMsgOpen				((uint8_t)0x07)
#define MagnetMsgClose				((uint8_t)0x08)


//二级消息类型（Pir）
#define PirMsgAck					((uint8_t)0x01)
#define PirMsgMute					((uint8_t)0x02)
#define PirMsgCancelMute			((uint8_t)0x03)
#define PirMsgTest					((uint8_t)0x04)
#define PirMsgFault					((uint8_t)0x05)
#define PirMsgLowBat				((uint8_t)0x06)
#define PirMsgBrokenInto			((uint8_t)0x07)
#define PirMsgGoAway				((uint8_t)0x08)


//二级消息类型（DriveWay）
#define DriveWayMsgAck				((uint8_t)0x01)
#define DriveWayMsgMute				((uint8_t)0x02)
#define DriveWayMsgCancelMute		((uint8_t)0x03)
#define DriveWayMsgTest				((uint8_t)0x04)
#define DriveWayMsgFault			((uint8_t)0x05)
#define DriveWayMsgLowBat			((uint8_t)0x06)
#define DriveWayMsgBrokenInto		((uint8_t)0x07)
#define DriveWayMsgGoAway			((uint8_t)0x08)


//二级消息类型（ClassBrokenAlarm）
#define GlassBrokenAlarmMsgAck			((uint8_t)0x01)
#define GlassBrokenAlarmMsgMute			((uint8_t)0x02)
#define GlassBrokenAlarmMsgCancelMute	((uint8_t)0x03)
#define GlassBrokenAlarmMsgTest			((uint8_t)0x04)
#define GlassBrokenAlarmMsgFault		((uint8_t)0x05)
#define GlassBrokenAlarmMsgLowBat		((uint8_t)0x06)
#define GlassBrokenAlarmMsgBrokenInto	((uint8_t)0x07)
#define GlassBrokenAlarmMsgGoAway		((uint8_t)0x08)

//二级消息类型（SoS）
#define SoSMsgAck					((uint8_t)0x01)
#define SoSMsgMute					((uint8_t)0x02)
#define SoSMsgCancelMute			((uint8_t)0x03)
#define SoSMsgTest					((uint8_t)0x04)
#define SoSMsgFault					((uint8_t)0x05)
#define SoSMsgLowBat				((uint8_t)0x06)
#define SoSMsgCallHelp				((uint8_t)0x07)
#define SoSMsgCancelCallHelp		((uint8_t)0x08)

//二级消息类型（Actuator）
#define ActuatorMsgAck				((uint8_t)0x01)
#define ActuatorMsgMute				((uint8_t)0x02)
#define ActuatorMsgCancelMute		((uint8_t)0x03)
#define ActuatorMsgTest				((uint8_t)0x04)
#define ActuatorMsgFault			((uint8_t)0x05)
#define ActuatorMsgLowBat			((uint8_t)0x06)
#define ActuatorMsgOutput			((uint8_t)0x07)
#define ActuatorMsgCancelOutput		((uint8_t)0x08)

//二级消息类型（KeyBoard）
#define KeyBoardMsgAck				((uint8_t)0x01)
#define KeyBoardMsgMute				((uint8_t)0x02)
#define KeyBoardMsgCancelMute		((uint8_t)0x03)
#define KeyBoardMsgTest				((uint8_t)0x04)
#define KeyBoardMsgFault			((uint8_t)0x05)
#define KeyBoardMsgLowBat			((uint8_t)0x06)
#define KeyBoardMsg_Key1			((uint8_t)0x07)
#define KeyBoardMsg_Key2			((uint8_t)0x08)
#define KeyBoardMsg_Key3			((uint8_t)0x09)
#define KeyBoardMsg_Key4			((uint8_t)0x0A)
#define KeyBoardMsg_Key5			((uint8_t)0x0B)
#define KeyBoardMsg_Key6			((uint8_t)0x0C)

//Rssi Level数据
#define RssiLevel_3		((uint8_t)0x01)
#define RssiLevel_2		((uint8_t)0x02)
#define RssiLevel_1		((uint8_t)0x03)
#define RssiLevel_0		((uint8_t)0x04)

//LowBat_Data Level电池电量
#define BatLevelLow			((uint8_t)0x01)
#define BatLevel3_1			((uint8_t)0x02)
#define BatLevel3_2			((uint8_t)0x03)
#define BatLevelFull		((uint8_t)0x04)

//Test_Data	测试消息数据
#define DevTestStart		((uint8_t)0x01)
#define DevTestStop			((uint8_t)0x02)
#define DevTestSuccess		((uint8_t)0x03)
#define DevTestFail			((uint8_t)0x04)

//Fault_Data 错误数据
#define DevFault			((uint8_t)0x01)
#define DevSmokeSenorFault	((uint8_t)0x02)
#define DevCoSenorFault		((uint8_t)0x03)
#define DevWaterSensorFault	((uint8_t)0x04)
#define DevTempFault		((uint8_t)0x05)
#define DevCancelFault		((uint8_t)0x06)

//Location Data数据
#ifdef SUPPORT_SPEAK_LOCATION
#define LocationMsgEntryway			((uint8_t)0x07)
#define LocationMsgBasement			((uint8_t)0x08)	
#define LocationMsgLivingroom		((uint8_t)0x09)
#define LocationMsgDiningroom		((uint8_t)0x0A)
#define LocationMsgKitchen			((uint8_t)0x0B)
#define LocationMsgHallway			((uint8_t)0x0C)
#define LocationMsgMasterbedroom	((uint8_t)0x0D)
#define LocationMsgKidsroom			((uint8_t)0x0E)
#define LocationMsgGuestroom		((uint8_t)0x0F)
#define LocationMsgDen				((uint8_t)0x10)
#define LocationMsgFamilyroom		((uint8_t)0x11)
#define LocationMsgStudy			((uint8_t)0x12)
#define LocationMsgOffice			((uint8_t)0x13)
#define LocationMsgWorkshop			((uint8_t)0x14)
#define LocationMsgUtilityroom		((uint8_t)0x15)
#define LocationMsgAttic			((uint8_t)0x16)
#endif

//playload帧结构
typedef struct _RadioMsgTag{
	uint8_t DeviceId[RfIdLength];
	uint8_t DeviceNumber;
	uint8_t TargetNumber;
	eRfDeviceType DeviceType;
	eHubMsgType MsgLevel1;
	uint8_t MsgLevel2;
	uint8_t MsgProperty;		//9
	uint8_t MsgData1;
	uint8_t MsgData2;	
	uint8_t MsgData3;
	uint8_t MsgData4;	
	uint8_t MsgCrc;
}RadioMsgTag;

//Send Rf Msg参数结构
typedef struct _RfSendMsgParamer
{
	uint8_t MsgTargetNum;
	eHubMsgType MsgLevel1Type;
	uint8_t MsgLevel2Type;
	uint8_t Property;
	uint8_t Data1;
	uint8_t Data2;
	uint8_t Data3;
	uint8_t Data4;
	eRadioInitMode Mode;
	uint8_t PackNum;
}RfSendMsgParamer;

//Send Rf Msg States结构体
typedef enum _eRfSendMsgStates
{
	eSendRfMsgIdle = 0x00,
	eSendRfMsgStart,
	eSendRfMsgSuccess,
	eSendRfMsgFail,
}eRfSendMsgStates;

//Search father node States结构体
typedef enum _eSearchFatherNodeStates
{
	eSearchNodeIdle = 0x00,
	eSearchNodeStart,
	eSearchNodeWait,
	eSearchNodeSuccess,
	eSearchNodeFail,
}eSearchFatherNodeStates;

//Rand Send Msg Property
typedef enum _eRandSendMsgProperty{
	eRandSendProperty_Null = 0,
	eRandSendProperty_Level_1,
	eRandSendProperty_Level_2,
	eRandSendProperty_Level_3,
	eRandSendProperty_Level_4,
	eRandSendProperty_Level_5,
	eRandSendProperty_Level_6,
	eRandSendProperty_Level_7,
	eRandSendProperty_Level_8,
	eRandSendProperty_Level_9,
	eRandSendProperty_Level_10,
	eRandSendProperty_Level_11,
	eRandSendProperty_Level_12,
	

	eRandSendProperty_Level_Max = 0xFF,
}eRandSendMsgProperty;

//Rand Send Msg State
typedef enum _eRandSendMsgState{
	eRandSend_Valid = 0,
	eRandSend_Invalid,
	eRandSend_Wait,
	eRandSend_Busy,
}eRandSendMsgState;



//子设备数据结构体
typedef struct _ChildDev{
	uint8_t SelfDevNum;
	uint8_t FatherDevNum;
	eRfDeviceType DevType;
	eProductType ProductType;
}ChildDev;

typedef struct _RadioDevTag{
	uint8_t RadioDevId[RfIdLength];
	uint8_t RadioUdid[RfIdLength];
	eRfDeviceType DevType;
	eRfDeviceType FatherDevType;
	eProductType ProductType;
	uint8_t SelfDevNum;
	uint8_t FatherDevNum;
	ePairedMode PairMode;
	eRandSendMsgProperty RandSendProperty;
	eRandSendMsgProperty RxRandSendProperty;
	eRandSendMsgState RandSendState;
}RadioDevTag;

extern RadioDevTag* pRadioDevTagObj;

void RadioApiTagInit(void);
RadioDevTag* GetRfApiTag(void);
ePairedMode GetRfPairedMode(void);
void RfHandleMsg(uint8_t DataOffset);
bool SaveIDProcess(uint8_t *pID, bool SaveIdFlag, uint8_t SavePairMode);

void HubPairMode(SystemApiTag  *me,QEvent *e);
void HubAlarmMode(SystemApiTag  *me,QEvent *e);
void HubInitialAlarmMode(SystemApiTag  *me,QEvent *e);
void HubRfUpdateHandleFunc(eHubMsgType MsgLevel1, uint8_t MsgLevel2, 
											uint8_t Value1, uint8_t Value2);

#ifdef SUPPORT_INTRANET_FUNCTION

#define SmokeAlarm				(0x01)
#define CoAlarm					(0x02)
#define GasAlarm				(0x03)

//一级消息类型
typedef enum _eInternatMsgType{
	eIntranetRadioMsgNull = 0x00,
	eIntranetRadioMsgAlarm,
	eIntranetRadioMsgCancleAlarm,
	eIntranetRadioMsgMute,
	eIntranetRadioMsgCancleMute,			//0x04
	eIntranetRadioMsgForwardTest,			//0x05
	eIntranetRadioMsgCancleTest,			//0x06
	eIntranetRadioMsgPairRequest,			//0x07
	eIntranetRadioMsgPairAck,				//0x08
	eIntranetRadioMsgAck,					//0x09	
	eIntranetRadioMsgOnlineCheck,			//0x0A
	eIntranetRadioMsgSync,					//0x0B	
	eIntranetRadioMsgCancleMsgSync,			//0x0C
	eIntranetRadioMsgFactoryPairRequest,	//0x0D
	eIntranetRadioMsgFactoryPairAck, 		//0x0E	
	eIntranetRadioMsgMasterTest,			//0x0F
	eIntranetRadioMsgStopMasterAlarm,		//0x10,
	eIntranetRadioMsgPairNewRequest,
	eIntranetRadioMsgPairNewPairAck,

	eIntranetRadioMsgSmokeAlarm = 0x41,
	eIntranetRadioMsgCoAlarm = 0x81,
	eIntranetRadioMsgGasAlarm = 0xC1,

#ifdef SUPPORT_SPEAK_LOCATION
	eIntranetRadioMsgLocationEntryway = 0xD1,//0xD1
	eIntranetRadioMsgLocationBasement,
	eIntranetRadioMsgLocationLivingroom,
	eIntranetRadioMsgLocationDiningroom,
	eIntranetRadioMsgLocationKitchen,
	eIntranetRadioMsgLocationHallway,
	eIntranetRadioMsgLocationMasterbedroom,
	eIntranetRadioMsgLocationKidsroom,
	eIntranetRadioMsgLocationGuestroom,
	eIntranetRadioMsgLocationDen,
	eIntranetRadioMsgLocationFamilyroom,
	eIntranetRadioMsgLocationStudy,
	eIntranetRadioMsgLocationOffice,
	eIntranetRadioMsgLocationWorkshop,
	eIntranetRadioMsgLocationUtilityroom,
	eIntranetRadioMsgLocationAttic,
#endif

}eInternatMsgType;

typedef struct _IntranetRadioMsgTag{
	uint8_t DeviceId[RfIdLength];
	eInternatMsgType MsgType;
	uint8_t MsgCrc;
}IntranetRadioMsgTag;

typedef struct _IntranetSendMsgParamer
{
	uint8_t PackNum;
	eInternatMsgType MsgType;
	eRadioInitMode Mode;
	bool ReSendFlag;
}IntranetSendMsgParamer;

void IntranetPairFactoryModeEvent(SystemApiTag  *me,QEvent *e);
void IntranetPairMasterModeEvent(SystemApiTag  *me,QEvent *e);
void IntranetPairSlaverModeEvent(SystemApiTag  *me,QEvent *e);
void IntranetBroadcastTestModeEvent(SystemApiTag  *me,QEvent *e);
void IntranetForwardTestModeEvent(SystemApiTag	*me,QEvent *e);
void IntranetBroadcastSmokeAlarmModeEvent(SystemApiTag	*me,QEvent *e);
void IntranetBroadcastCoAlarmModeEvent(SystemApiTag  *me,QEvent *e);
void IntranetForwardSmokeAlarmModeEvent(SystemApiTag  *me,QEvent *e);
void IntranetForwardCoAlarmModeEvent(SystemApiTag  *me,QEvent *e);
#endif


void RFTestHandle(SystemApiTag  *me,QEvent *e);
void RadioService(void);


#ifdef __cplusplus
}
#endif



#endif

