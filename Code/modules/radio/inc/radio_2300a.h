#ifndef	_RADIO_2300A_H_
#define _RADIO_2300A_H_

#include "base_types.h"

#ifdef __cplusplus 
extern "C" { 
#endif

#define HUB_PLAYLOAD_LEN		((uint8_t)15)
#define INTRANET_PLAYLOAD_LEN	((uint8_t)6)

/* RF state machine */
typedef enum {
	RF_STATE_IDLE = 0,
	RF_STATE_RX_START,
	RF_STATE_RX_WAIT,
	RF_STATE_RX_DONE,
	RF_STATE_RX_TIMEOUT,
	RF_STATE_TX_START,
	RF_STATE_TX_WAIT,
	RF_STATE_TX_DONE,
	RF_STATE_TX_TIMEOUT,
	RF_STATE_ERROR,
}eRFStatus;

/* RF process function results */
typedef enum {
	RF_IDLE = 0,
	RF_BUSY,
	RF_RX_DONE,
	RF_RX_TIMEOUT,
	RF_TX_DONE,
	RF_TX_TIMEOUT,
	RF_ERROR,
}eRFResult;

typedef enum{
	eRadioCfgTxMode = 0,
	eRadioCfgRxMode,
	eRadioCfgTestFskTxMode,
	eRadioCfgTestOokTxMode,
	eRadioCfgTestRxMode,
}eRadioCfgMode;

//radio rf mode
typedef enum _eRadioInitMode{
	LowPowerHubMode = 0,
	NormalHubMode,
	LowPowerIntranetMode,
	NormalIntranetMode,
	NormalIntranetSpecialMode,
	FactoryPairMode,

	FskTestMode,
	OokTestMode,	
	RxTestMode,
}eRadioInitMode;

struct strRadio
{
	uint8_t PlayloadLen;
	eRadioInitMode InitMode;
	void (*RadioInit)(eRadioInitMode InitMode);
	void (*RadioConfig)(eRadioCfgMode CfgMode);
	void (*RadioSetStates)(eRFStatus States);
	eRFStatus (*RadioGetStates)(void);
	uint8_t (*RadioGetIrqFlag)(void);
	void (*RadioSend)(uint8_t buf[], uint16_t len, uint16_t packnum, uint32_t timeout);
	void (*WaitSendDone)(void);
	bool (*RadioRx)(uint8_t buf[], uint16_t len);
	eRFResult (*RadioProcess)(void);
};

extern struct strRadio Radio;

#ifdef __cplusplus
}
#endif

#endif

