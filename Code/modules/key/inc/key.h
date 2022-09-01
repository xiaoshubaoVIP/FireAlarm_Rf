#ifndef	_KEY_H_
#define _KEY_H_

#include "common.h"
#include "base_types.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define	Key_1_Port		(0x02)
#define	Key_1_Pin		(0x07)


typedef enum{
	KEY1_PRESS = 0x01,
	KEY2_PRESS = 0x02,
	KEY3_PRESS = 0x04,
	KEY4_PRESS = 0x08,
	KEY5_PRESS = 0x10,

	KEY_DEFAULT = 0x80,
}KEYValue;


typedef enum{
	TYPE_NONE = 0x00,
	TYPE_LONG_CLICK,
	TYPE_WAIT,
	TYPE_SHORT_CLICK,
	TYPE_DOUBLE_CLICK,
	TYPE_THREE_CLICK,
	TYPE_FOUR_CLICK,
}KEYType;


typedef struct _KeyApi{
	uint16_t keyCounter;	  
	uint8_t keyClickFlag;	
	KEYType keyType;	
	KEYValue keyCurrent;
	KEYValue keyValue;
	KEYValue keyValueLast;
	uint8_t keyClickCnt;
}KeyApi;

void KeyInit(void);
void PollingKeyScan(TmrHdl Handle, eTmrCmd eCmd);
void KeyHandleMsg(void);

#ifdef __cplusplus
}
#endif



#endif

