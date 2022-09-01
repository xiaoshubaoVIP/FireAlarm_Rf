#ifndef	_APP_UART_H_
#define _APP_UART_H_


#ifdef __cplusplus
extern "C"
{
#endif


#define	UART_INT_is_L() 	(HalGpioRead(PIN_UART_INT) == PIN_RESET)
#define	UART_INT_is_H()		(HalGpioRead(PIN_UART_INT) == PIN_SET)

//[0xAF][MsgType][Length][Data...][Crc]
//  1     1       1        N        1  

#define UART_TX_FRAME_HEADER   		(0xAF)
#define UART_MSG_FIXED_LENGTH		(0x04)
#define UART_MSG_MAX_DATA_LENGTH	(0x0F)

#define UART_MSG_HEAD		0
#define UART_MSG_TYPE		1
#define UART_MSG_LENGTH		2
#define UART_MSG_DATA_START	3


#define	UartMsg_Int_Port  			(0x03)
#define	UartMsg_Int_Pin	 			(0x06)

//一级消息类型
typedef enum _eUartMsgType{
	eUartMsgNull = 0x00,
	eUartMsgPair,
	eUartMsgRfStates,
	eUartMsgFireAlarm,
	eUartMsgWaterSensor,
	eUartMsgPlug,
	eUartMsgRomote,
	eUartMsgPir,
	eUartMsgMagnet,
	eUartMsgDriveWay,
	eUartMsgGlassBrokenAlarm,
	eUartMsgSos,
	eUartMsgActuator,
	eUartMsgKeyBoard,
	eUartMsgDoorBell,
	eUartMsgHorn,
	eUartMsgCamera,
	eUartMsgWeatherStation,
}eUartMsgType;


//Pair二级消息类型
#define UartMsgPairAck 				((uint8_t)0x01)
#define UartMsgPairResetId			((uint8_t)0x02)
#define UartMsgPairFactoryMode		((uint8_t)0x03)
#define UartMsgPairHubMode	 		((uint8_t)0x04)
#define UartMsgPairSlaverMode	 	((uint8_t)0x05)
#define UartMsgPairMasterMode	 	((uint8_t)0x06)
#define UartMsgPairSuccess			((uint8_t)0x07)
#define UartMsgPairFail				((uint8_t)0x08)
#define UartMsgPairStop				((uint8_t)0x09)

//Rf states二级消息类型
#define UartMsgRfStatesAck 			((uint8_t)0x01)
#define UartMsgRfStatesOnline		((uint8_t)0x02)
#define UartMsgRfStatesOffline		((uint8_t)0x03)

//FireAlarm二级消息类型
#define UartFireAlarmMsgAck			((uint8_t)0x01)
#define UartFireAlarmMsgMute		((uint8_t)0x02)
#define UartFireAlarmMsgCancelMute	((uint8_t)0x03)
#define UartFireAlarmMsgTest		((uint8_t)0x04)
#define UartFireAlarmMsgFault		((uint8_t)0x05)
#define UartFireAlarmMsgLowBat		((uint8_t)0x06)
#define UartFireAlarmMsgSmokeAlarm	((uint8_t)0x07)
#define UartFireAlarmMsgCoAlarm		((uint8_t)0x08)
#define UartFireAlarmMsgHeatAlarm	((uint8_t)0x09)
#define UartFireAlarmMsgGasAlarm	((uint8_t)0x0A)
#define UartFireAlarmMsgCancelAlarm	((uint8_t)0x0B)
#define UartFireAlarmMsgUpdateCoPpm	((uint8_t)0x0C)


//WaterSensor二级消息类型
#define UartWaterSensorMsgAck			((uint8_t)0x01)
#define UartWaterSensorMsgMute			((uint8_t)0x02)
#define UartWaterSensorMsgCancelMute	((uint8_t)0x03)
#define UartWaterSensorMsgTest			((uint8_t)0x04)
#define UartWaterSensorMsgFault			((uint8_t)0x05)
#define UartWaterSensorMsgLowBat		((uint8_t)0x06)
#define UartWaterSensorMsgAlarm			((uint8_t)0x07)
#define UartWaterSensorMsgCancelAlarm	((uint8_t)0x08)

//Plug二级消息类型
#define UartPlugMsgAck					((uint8_t)0x01)
#define UartPlugMsgMute					((uint8_t)0x02)
#define UartPlugMsgCancelMute			((uint8_t)0x03)
#define UartPlugMsgTest					((uint8_t)0x04)
#define UartPlugMsgFault				((uint8_t)0x05)
#define UartPlugMsgLowBat				((uint8_t)0x06)
#define UartPlugMsgTurnOn				((uint8_t)0x07)
#define UartPlugMsgTurnOff				((uint8_t)0x08)

//Romote二级消息类型
#define UartRomoteMsgAck				((uint8_t)0x01)
#define UartRomoteMsgMute				((uint8_t)0x02)
#define UartRomoteMsgCancelMute			((uint8_t)0x03)
#define UartRomoteMsgTest				((uint8_t)0x04)
#define UartRomoteMsgFault				((uint8_t)0x05)
#define UartRomoteMsgLowBat				((uint8_t)0x06)
#define UartRomoteMsg_Key1				((uint8_t)0x07)
#define UartRomoteMsg_Key2				((uint8_t)0x08)
#define UartRomoteMsg_Key3				((uint8_t)0x09)
#define UartRomoteMsg_Key4				((uint8_t)0x0A)

//Pir二级消息类型
#define UartPirMsgAck					((uint8_t)0x01)
#define UartPirMsgMute					((uint8_t)0x02)
#define UartPirMsgCancelMute			((uint8_t)0x03)
#define UartPirMsgTest					((uint8_t)0x04)
#define UartPirMsgFault					((uint8_t)0x05)
#define UartPirMsgLowBat				((uint8_t)0x06)
#define UartPirMsgBrokenInto			((uint8_t)0x07)
#define UartPirMsgGoAway				((uint8_t)0x08)
#define UartPirMsgAntiDemolition		((uint8_t)0x09)

//Magnet二级消息类型
#define UartMagnetMsgAck				((uint8_t)0x01)
#define UartMagnetMsgMute				((uint8_t)0x02)
#define UartMagnetMsgCancelMute			((uint8_t)0x03)
#define UartMagnetMsgTest				((uint8_t)0x04)
#define UartMagnetMsgFault				((uint8_t)0x05)
#define UartMagnetMsgLowBat				((uint8_t)0x06)
#define UartMagnetMsgOpen				((uint8_t)0x07)
#define UartMagnetMsgClose				((uint8_t)0x08)

//DriveWay二级消息类型
#define UartDriveWayMsgAck				((uint8_t)0x01)
#define UartDriveWayMsgMute				((uint8_t)0x02)
#define UartDriveWayMsgCancelMute		((uint8_t)0x03)
#define UartDriveWayMsgTest				((uint8_t)0x04)
#define UartDriveWayMsgFault			((uint8_t)0x05)
#define UartDriveWayMsgLowBat			((uint8_t)0x06)
#define UartDriveWayMsgBrokenInto		((uint8_t)0x07)
#define UartDriveWayMsgGoAway			((uint8_t)0x08)

//GlassBrokenAlarm二级消息类型
#define UartGlassBrokenAlarmMsgAck			((uint8_t)0x01)
#define UartGlassBrokenAlarmMsgMute			((uint8_t)0x02)
#define UartGlassBrokenAlarmMsgCancelMute	((uint8_t)0x03)
#define UartGlassBrokenAlarmMsgTest			((uint8_t)0x04)
#define UartGlassBrokenAlarmMsgFault		((uint8_t)0x05)
#define UartGlassBrokenAlarmMsgLowBat		((uint8_t)0x06)
#define UartGlassBrokenAlarmMsgBrokenInto	((uint8_t)0x07)
#define UartGlassBrokenAlarmMsgGoAway		((uint8_t)0x08)

//SoS二级消息类型
#define UartSosMsgAck					((uint8_t)0x01)
#define UartSosMsgMute					((uint8_t)0x02)
#define UartSosMsgCancelMute			((uint8_t)0x03)
#define UartSosMsgTest					((uint8_t)0x04)
#define UartSosMsgFault					((uint8_t)0x05)
#define UartSosMsgLowBat				((uint8_t)0x06)
#define UartSosMsgCallHelp				((uint8_t)0x07)
#define UartSosMsgCancelCallHelp		((uint8_t)0x08)

//Actuator二级消息类型
#define UartActuatorMsgAck				((uint8_t)0x01)
#define UartActuatorMsgMute				((uint8_t)0x02)
#define UartActuatorMsgCancelMute		((uint8_t)0x03)
#define UartActuatorMsgTest				((uint8_t)0x04)
#define UartActuatorMsgFault			((uint8_t)0x05)
#define UartActuatorMsgLowBat			((uint8_t)0x06)
#define UartActuatorMsgOutput			((uint8_t)0x07)
#define UartActuatorMsgCancelOutput		((uint8_t)0x08)

//KeyBoard二级消息类型
#define UartKeyBoardMsgAck				((uint8_t)0x01)
#define UartKeyBoardMsgMute				((uint8_t)0x02)
#define UartKeyBoardMsgCancelMute		((uint8_t)0x03)
#define UartKeyBoardMsgTest				((uint8_t)0x04)
#define UartKeyBoardMsgFault			((uint8_t)0x05)
#define UartKeyBoardMsgLowBat			((uint8_t)0x06)
#define UartKeyBoardMsg_Key1			((uint8_t)0x07)
#define UartKeyBoardMsg_Key2			((uint8_t)0x08)
#define UartKeyBoardMsg_Key3			((uint8_t)0x09)
#define UartKeyBoardMsg_Key4			((uint8_t)0x0A)
#define UartKeyBoardMsg_Key5			((uint8_t)0x0B)
#define UartKeyBoardMsg_Key6			((uint8_t)0x0C)
#define UartKeyBoardMsg_Key7			((uint8_t)0x0D)
#define UartKeyBoardMsg_Key8			((uint8_t)0x0E)
#define UartKeyBoardMsg_Key9			((uint8_t)0x0F)
#define UartKeyBoardMsg_Key10			((uint8_t)0x10)

//DoolBell二级消息类型
#define UartDoorBellMsgAck				((uint8_t)0x01)
#define UartDoorBellMsgMute				((uint8_t)0x02)
#define UartDoorBellMsgCancelMute		((uint8_t)0x03)
#define UartDoorBellMsgTest				((uint8_t)0x04)
#define UartDoorBellMsgFault			((uint8_t)0x05)
#define UartDoorBellMsgLowBat			((uint8_t)0x06)
#define UartDoorBellMsgOn				((uint8_t)0x07)
#define UartDoorBellMsgOff				((uint8_t)0x08)

//Horn二级消息类型
#define UartHornMsgAck				((uint8_t)0x01)
#define UartHornMsgMute				((uint8_t)0x02)
#define UartHornMsgCancelMute		((uint8_t)0x03)
#define UartHornMsgTest				((uint8_t)0x04)
#define UartHornMsgFault			((uint8_t)0x05)
#define UartHornMsgLowBat			((uint8_t)0x06)
#define UartHornMsgOn				((uint8_t)0x07)
#define UartHornMsgOff				((uint8_t)0x08)

//Camera二级消息类型
#define UartCameraMsgAck			((uint8_t)0x01)
#define UartCameraMsgMute			((uint8_t)0x02)
#define UartCameraMsgCancelMute		((uint8_t)0x03)
#define UartCameraMsgTest			((uint8_t)0x04)
#define UartCameraMsgFault			((uint8_t)0x05)
#define UartCameraMsgLowBat			((uint8_t)0x06)
#define UartCameraMsgOn				((uint8_t)0x07)
#define UartCameraMsgOff			((uint8_t)0x08)

//WeatherStation二级消息类型
#define UartWeatherStationMsgAck			((uint8_t)0x01)
#define UartWeatherStationMsgMute			((uint8_t)0x02)
#define UartWeatherStationMsgCancelMute		((uint8_t)0x03)
#define UartWeatherStationMsgTest			((uint8_t)0x04)
#define UartWeatherStationMsgFault			((uint8_t)0x05)
#define UartWeatherStationMsgLowBat			((uint8_t)0x06)
#define UartWeatherStationMsgOn				((uint8_t)0x07)
#define UartWeatherStationMsgOff			((uint8_t)0x08)

//Send Uart Msg参数结构
typedef struct _UartSendMsgParamer
{
	uint8_t MsgLevel1;
	eUartMsgType MsgType;
	uint8_t MsgData;
}UartSendMsgParamer;


typedef struct _UartApi{
	bool UartExtIntFlag;
}UartApi;


void UartApiTagInit(void);
void UartReceiveInput(unsigned char value);
void UartService(void);
void UartDownLoadHandleFunc(eUartMsgType MsgType, uint8_t Value1, uint8_t Value2);



#ifdef __cplusplus
}
#endif



#endif

