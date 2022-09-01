#ifndef __CONFIG_H__
#define __CONFIG_H__

#define SoftWareVer  "[FireAlarm](New_Driver_3th) Ver=1.1.1"


//#define SUPPORT_HC32L100
#define SUPPORT_BAT32G133

//#define SUPPORT_FCC_TESE

#define SUPPORT_X_SENSE_EN
//#define SUPPORT_X_SENSE_UL
//#define SUPPORT_AEGISLINK_EN 
//#define SUPPORT_AEGISLINK_UL
//#define SUPPORT_RADIO_TEST


#define SUPPORT_INTRANET_FUNCTION
#ifdef SUPPORT_INTRANET_FUNCTION
#define SUPPORT_SPEAK_LOCATION
#endif

//²úÆ·´úºÅProduction type
typedef enum _eProductType
{
	eProductType_NULL = 0x00,
	eProductType_Hub1,
	eProductType_Hub2,
	eProductType_Hub3,
	eProductType_Repeater1_Smart,
	eProductType_Repeater2_Smart,
	eProductType_Repeater3_Smart, 
	eProductType_SC01_Smart,
	eProductType_SC03_Smart,
	eProductType_SD01_Smart,
	eProductType_SD03_Smart,
	eProductType_CD01_Smart,
	eProductType_CD03_Smart,
	eProductType_CD14_Smart,
	eProductType_XC01_Smart,
	eProductType_XC02_Smart,
	eProductType_XC03_Smart,
	eProductType_XP01_Smart,
	eProductType_XP01_S_Smart,
	eProductType_XP02_Smart,
	eProductType_XP02_S_Smart,
	eProductType_XP03_Smart,
	eProductType_XP03_S_Smart,
	eProductType_XS01_Smart,
	eProductType_XS02_Smart,
	eProductType_XS03_Smart,
	eProductType_HC01_Smart,
	eProductType_HC02_Smart,
	eProductType_HC03_Smart,
	eProductType_GC01_Smart,
	eProductType_GC02_Smart,
	eProductType_GC03_Smart,
	eProductType_Heat01_Smart,
	eProductType_Heat02_Smart,
	eProductType_Heat03_Smart,
	eProductType_Gas01_Smart,
	eProductType_Gas02_Smart,
	eProductType_Gas03_Smart,
	eProductType_Water01_Smart,
	eProductType_Water02_Smart,
	eProductType_Water03_Smart,
	eProductType_Plug01_Smart,
	eProductType_Plug02_Smart,
	eProductType_Plug03_Smart,
	eProductType_Romote01_Smart,
	eProductType_Romote02_Smart,
	eProductType_Romote03_Smart,
	eProductType_PIR01_Smart,
	eProductType_PIR02_Smart,
	eProductType_PIR03_Smart,
	eProductType_Magnets01_Smart,
	eProductType_Magnets02_Smart,
	eProductType_Magnets03_Smart,
	eProductType_DriveWay01_Smart,
	eProductType_DriveWay02_Smart,
	eProductType_DriveWay03_Smart,
	eProductType_GlassAlarm01_Smart,
	eProductType_GlassAlarm02_Smart,
	eProductType_GlassAlarm03_Smart,
	eProductType_SOS01_Smart,
	eProductType_SOS02_Smart,
	eProductType_SOS03_Smart,
	eProductType_Actuator01_Smart,
	eProductType_Actuator02_Smart,
	eProductType_Actuator03_Smart,
	eProductType_KeyBoard01_Smart,
	eProductType_KeyBoard02_Smart,
	eProductType_KeyBoard03_Smart,
	eProductType_DoorBell01_Smart,
	eProductType_DoorBell02_Smart,
	eProductType_DoorBell03_Smart,
	eProductType_Horn01_Smart,
	eProductType_Horn02_Smart,
	eProductType_Horn03_Smart,
	eProductType_Camera01_Smart,
	eProductType_Camera02_Smart,
	eProductType_Camera03_Smart,
	eProductType_WeatherStation01_Smart,
	eProductType_WeatherStation02_Smart,
	eProductType_WeatherStation03_Smart,
}eProductType;


#endif
