/*
 * THE FOLLOWING FIRMWARE IS PROVIDED: (1) "AS IS" WITH NO WARRANTY; AND
 * (2)TO ENABLE ACCESS TO CODING INFORMATION TO GUIDE AND FACILITATE CUSTOMER.
 * CONSEQUENTLY, CMOSTEK SHALL NOT BE HELD LIABLE FOR ANY DIRECT, INDIRECT OR
 * CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE CONTENT
 * OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING INFORMATION
 * CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
 *
 * Copyright (C) CMOSTEK SZ.
 */

/*!
 * @file    radio.c
 * @brief   Generic radio handlers
 *
 * @version 1.2
 * @date    Jul 17 2017
 * @author  CMOSTEK R@D
 */
#include <string.h>
#include "config.h"
#include "radio_2300a.h"
#include "radio_2300a_driver.h"
#include "system.h"
#include "debug.h"

#if defined(SUPPORT_X_SENSE_EN) 
#include "2300a_params_x_sense_en.h"
#elif defined(SUPPORT_X_SENSE_UL)
#include "2300a_params_x_sense_ul.h"
#elif defined(SUPPORT_AEGISLINK_EN)
#include "2300a_params_aegislink_en.h"
#elif defined(SUPPORT_AEGISLINK_UL)
#include "2300a_params_aegislink_ul.h"
#elif defined(SUPPORT_RADIO_TEST)
#include "2300a_params_radio_test.h"
#endif

static eRFStatus g_nNextRFState = RF_STATE_IDLE;
static uint8_t* g_pRxBuffer = NULL;
static uint8_t* g_pTxBuffer = NULL;
static uint16_t g_nRxLength = 0;
static uint16_t g_nTxLength = 0;

static uint32_t  g_nRxTimeout = INFINITE;
static uint32_t g_nTxTimeout = INFINITE;
static uint32_t g_nRxTimeCount = 0;
static uint32_t g_nTxTimeCount = 0;

static uint8_t g_nInterrutFlags = 0;

#define SEND_MSG_TIMEOUT	((uint16_t)5200)
#define ENABLE_ANTENNA_SWITCH

void CMT2300Init(eRadioInitMode eInitMode);
void RF_SetStatus(eRFStatus nStatus);
eRFStatus RF_GetStatus(void);
uint8_t RF_GetInterruptFlags(void);
void RF_Config(eRadioCfgMode RadioMode);
void RF_Send(uint8_t buf[], uint16_t len, uint16_t packnum, uint32_t timeout);
void RF_WaitSendDone(void);
bool RF_Rx(uint8_t buf[], uint16_t len);
eRFResult RF_Process(void);


struct strRadio Radio = 
{
	HUB_PLAYLOAD_LEN,
	LowPowerHubMode,
	CMT2300Init,
	RF_Config,
	RF_SetStatus,
	RF_GetStatus,
	RF_GetInterruptFlags,
	RF_Send,
	RF_WaitSendDone,
	RF_Rx,
	RF_Process,
};


void CMT2300Init(eRadioInitMode eInitMode)
{
	debug_printf("RF mode=%d\n", eInitMode);

	Radio.InitMode = eInitMode;

	CMT2300A_InitGpio();
	CMT2300A_Init();

	/* Config registers */	
	switch(eInitMode)
	{
		case LowPowerHubMode:
			Radio.PlayloadLen = HUB_PLAYLOAD_LEN;
			CMT2300A_ConfigRegBank(CMT2300A_CMT_BANK_ADDR		, Hub_CMT_Bank_LowPower			, CMT2300A_CMT_BANK_SIZE);
			CMT2300A_ConfigRegBank(CMT2300A_SYSTEM_BANK_ADDR	, Hub_System_Bank_LowPower		, CMT2300A_SYSTEM_BANK_SIZE);
			CMT2300A_ConfigRegBank(CMT2300A_FREQUENCY_BANK_ADDR , Hub_Frequency_Bank_LowPower	, CMT2300A_FREQUENCY_BANK_SIZE);
			CMT2300A_ConfigRegBank(CMT2300A_DATA_RATE_BANK_ADDR , Hub_Data_Rate_Bank_LowPower	, CMT2300A_DATA_RATE_BANK_SIZE);
			CMT2300A_ConfigRegBank(CMT2300A_BASEBAND_BANK_ADDR	, Hub_Baseband_Bank_LowPower	, CMT2300A_BASEBAND_BANK_SIZE);
			CMT2300A_ConfigRegBank(CMT2300A_TX_BANK_ADDR		, Hub_TX_Bank_LowPower			, CMT2300A_TX_BANK_SIZE);
			RF_Config(eRadioCfgRxMode);
			break;

		case NormalHubMode:
			Radio.PlayloadLen = HUB_PLAYLOAD_LEN;
			CMT2300A_ConfigRegBank(CMT2300A_CMT_BANK_ADDR		, Hub_CMT_Bank_Normal			, CMT2300A_CMT_BANK_SIZE);
			CMT2300A_ConfigRegBank(CMT2300A_SYSTEM_BANK_ADDR	, Hub_System_Bank_Normal		, CMT2300A_SYSTEM_BANK_SIZE);
			CMT2300A_ConfigRegBank(CMT2300A_FREQUENCY_BANK_ADDR , Hub_Frequency_Bank_Normal		, CMT2300A_FREQUENCY_BANK_SIZE);
			CMT2300A_ConfigRegBank(CMT2300A_DATA_RATE_BANK_ADDR , Hub_Data_Rate_Bank_Normal		, CMT2300A_DATA_RATE_BANK_SIZE);
			CMT2300A_ConfigRegBank(CMT2300A_BASEBAND_BANK_ADDR	, Hub_Baseband_Bank_Normal		, CMT2300A_BASEBAND_BANK_SIZE);
			CMT2300A_ConfigRegBank(CMT2300A_TX_BANK_ADDR		, Hub_TX_Bank_Normal			, CMT2300A_TX_BANK_SIZE);
			RF_Config(eRadioCfgRxMode);
			break;

		case LowPowerIntranetMode:
			Radio.PlayloadLen = INTRANET_PLAYLOAD_LEN;
			CMT2300A_ConfigRegBank(CMT2300A_CMT_BANK_ADDR		, Intranet_CMT_Bank_LowPower		, CMT2300A_CMT_BANK_SIZE);
			CMT2300A_ConfigRegBank(CMT2300A_SYSTEM_BANK_ADDR	, Intranet_System_Bank_LowPower		, CMT2300A_SYSTEM_BANK_SIZE);
			CMT2300A_ConfigRegBank(CMT2300A_FREQUENCY_BANK_ADDR , Intranet_Frequency_Bank_LowPower	, CMT2300A_FREQUENCY_BANK_SIZE);
			CMT2300A_ConfigRegBank(CMT2300A_DATA_RATE_BANK_ADDR , Intranet_Data_Rate_Bank_LowPower	, CMT2300A_DATA_RATE_BANK_SIZE);
			CMT2300A_ConfigRegBank(CMT2300A_BASEBAND_BANK_ADDR	, Intranet_Baseband_Bank_LowPower	, CMT2300A_BASEBAND_BANK_SIZE);
			CMT2300A_ConfigRegBank(CMT2300A_TX_BANK_ADDR		, Intranet_TX_Bank_LowPower			, CMT2300A_TX_BANK_SIZE);
			RF_Config(eRadioCfgRxMode);
			break;

		case NormalIntranetMode:
			Radio.PlayloadLen = INTRANET_PLAYLOAD_LEN;
			CMT2300A_ConfigRegBank(CMT2300A_CMT_BANK_ADDR		, Intranet_CMT_Bank_Normal			, CMT2300A_CMT_BANK_SIZE);
			CMT2300A_ConfigRegBank(CMT2300A_SYSTEM_BANK_ADDR	, Intranet_System_Bank_Normal		, CMT2300A_SYSTEM_BANK_SIZE);
			CMT2300A_ConfigRegBank(CMT2300A_FREQUENCY_BANK_ADDR , Intranet_Frequency_Bank_Normal	, CMT2300A_FREQUENCY_BANK_SIZE);
			CMT2300A_ConfigRegBank(CMT2300A_DATA_RATE_BANK_ADDR , Intranet_Data_Rate_Bank_Normal	, CMT2300A_DATA_RATE_BANK_SIZE);
			CMT2300A_ConfigRegBank(CMT2300A_BASEBAND_BANK_ADDR	, Intranet_Baseband_Bank_Normal		, CMT2300A_BASEBAND_BANK_SIZE);
			CMT2300A_ConfigRegBank(CMT2300A_TX_BANK_ADDR		, Intranet_TX_Bank_Normal			, CMT2300A_TX_BANK_SIZE);
			RF_Config(eRadioCfgRxMode);
			break;

#if defined(SUPPORT_X_SENSE_EN) || defined(SUPPORT_X_SENSE_UL)
		case NormalIntranetSpecialMode:
			Radio.PlayloadLen = INTRANET_PLAYLOAD_LEN;
			CMT2300A_ConfigRegBank(CMT2300A_CMT_BANK_ADDR		, Intranet_CMT_Bank_Normal_Specail			, CMT2300A_CMT_BANK_SIZE);
			CMT2300A_ConfigRegBank(CMT2300A_SYSTEM_BANK_ADDR	, Intranet_System_Bank_Normal_Specail		, CMT2300A_SYSTEM_BANK_SIZE);
			CMT2300A_ConfigRegBank(CMT2300A_FREQUENCY_BANK_ADDR , Intranet_Frequency_Bank_Normal_Specail	, CMT2300A_FREQUENCY_BANK_SIZE);
			CMT2300A_ConfigRegBank(CMT2300A_DATA_RATE_BANK_ADDR , Intranet_Data_Rate_Bank_Normal_Specail	, CMT2300A_DATA_RATE_BANK_SIZE);
			CMT2300A_ConfigRegBank(CMT2300A_BASEBAND_BANK_ADDR	, Intranet_Baseband_Bank_Normal_Specail		, CMT2300A_BASEBAND_BANK_SIZE);
			CMT2300A_ConfigRegBank(CMT2300A_TX_BANK_ADDR		, Intranet_TX_Bank_Normal_Specail			, CMT2300A_TX_BANK_SIZE);
			RF_Config(eRadioCfgRxMode);
			break;
#endif

		case FactoryPairMode:
			Radio.PlayloadLen = INTRANET_PLAYLOAD_LEN;
			CMT2300A_ConfigRegBank(CMT2300A_CMT_BANK_ADDR		, Factory_Pair_CMT_Bank				, CMT2300A_CMT_BANK_SIZE);
			CMT2300A_ConfigRegBank(CMT2300A_SYSTEM_BANK_ADDR	, Factory_Pair_System_Bank			, CMT2300A_SYSTEM_BANK_SIZE);
			CMT2300A_ConfigRegBank(CMT2300A_FREQUENCY_BANK_ADDR , Factory_Pair_Frequency_Bank		, CMT2300A_FREQUENCY_BANK_SIZE);
			CMT2300A_ConfigRegBank(CMT2300A_DATA_RATE_BANK_ADDR , Factory_Pair_Data_Rate_Bank		, CMT2300A_DATA_RATE_BANK_SIZE);
			CMT2300A_ConfigRegBank(CMT2300A_BASEBAND_BANK_ADDR	, Factory_Pair_Baseband_Bank		, CMT2300A_BASEBAND_BANK_SIZE);
			CMT2300A_ConfigRegBank(CMT2300A_TX_BANK_ADDR		, Factory_Pair_TX_Bank				, CMT2300A_TX_BANK_SIZE);
			RF_Config(eRadioCfgRxMode);
			break;

			
		case FskTestMode:
			Radio.PlayloadLen = INTRANET_PLAYLOAD_LEN;
			CMT2300A_ConfigRegBank(CMT2300A_CMT_BANK_ADDR		, FSK_Test_CMT_Bank				, CMT2300A_CMT_BANK_SIZE);
			CMT2300A_ConfigRegBank(CMT2300A_SYSTEM_BANK_ADDR	, FSK_Test_System_Bank			, CMT2300A_SYSTEM_BANK_SIZE);
			CMT2300A_ConfigRegBank(CMT2300A_FREQUENCY_BANK_ADDR , FSK_Test_Frequency_Bank		, CMT2300A_FREQUENCY_BANK_SIZE);
			CMT2300A_ConfigRegBank(CMT2300A_DATA_RATE_BANK_ADDR , FSK_Test_Data_Rate_Bank		, CMT2300A_DATA_RATE_BANK_SIZE);
			CMT2300A_ConfigRegBank(CMT2300A_BASEBAND_BANK_ADDR	, FSK_Test_Baseband_Bank		, CMT2300A_BASEBAND_BANK_SIZE);
			CMT2300A_ConfigRegBank(CMT2300A_TX_BANK_ADDR		, FSK_Test_TX_Bank				, CMT2300A_TX_BANK_SIZE);
			RF_Config(eRadioCfgTestFskTxMode);
			break;

		case OokTestMode:
			Radio.PlayloadLen = INTRANET_PLAYLOAD_LEN;
			CMT2300A_ConfigRegBank(CMT2300A_CMT_BANK_ADDR		, OOK_Test_CMT_Bank				, CMT2300A_CMT_BANK_SIZE);
			CMT2300A_ConfigRegBank(CMT2300A_SYSTEM_BANK_ADDR	, OOK_Test_System_Bank			, CMT2300A_SYSTEM_BANK_SIZE);
			CMT2300A_ConfigRegBank(CMT2300A_FREQUENCY_BANK_ADDR , OOK_Test_Frequency_Bank		, CMT2300A_FREQUENCY_BANK_SIZE);
			CMT2300A_ConfigRegBank(CMT2300A_DATA_RATE_BANK_ADDR , OOK_Test_Data_Rate_Bank		, CMT2300A_DATA_RATE_BANK_SIZE);
			CMT2300A_ConfigRegBank(CMT2300A_BASEBAND_BANK_ADDR	, OOK_Test_Baseband_Bank		, CMT2300A_BASEBAND_BANK_SIZE);
			CMT2300A_ConfigRegBank(CMT2300A_TX_BANK_ADDR		, OOK_Test_TX_Bank				, CMT2300A_TX_BANK_SIZE);
			RF_Config(eRadioCfgTestOokTxMode);
			break;
		
		case RxTestMode:
			Radio.PlayloadLen = INTRANET_PLAYLOAD_LEN;
			CMT2300A_ConfigRegBank(CMT2300A_CMT_BANK_ADDR		, RX_Test_CMT_Bank				, CMT2300A_CMT_BANK_SIZE);
			CMT2300A_ConfigRegBank(CMT2300A_SYSTEM_BANK_ADDR	, RX_Test_System_Bank			, CMT2300A_SYSTEM_BANK_SIZE);
			CMT2300A_ConfigRegBank(CMT2300A_FREQUENCY_BANK_ADDR , RX_Test_Frequency_Bank		, CMT2300A_FREQUENCY_BANK_SIZE);
			CMT2300A_ConfigRegBank(CMT2300A_DATA_RATE_BANK_ADDR , RX_Test_Data_Rate_Bank		, CMT2300A_DATA_RATE_BANK_SIZE);
			CMT2300A_ConfigRegBank(CMT2300A_BASEBAND_BANK_ADDR	, RX_Test_Baseband_Bank			, CMT2300A_BASEBAND_BANK_SIZE);
			CMT2300A_ConfigRegBank(CMT2300A_TX_BANK_ADDR		, RX_Test_TX_Bank				, CMT2300A_TX_BANK_SIZE);
			RF_Config(eRadioCfgTestRxMode);
			break;
	}
}


void RF_Config(eRadioCfgMode CfgMode)
{
	CMT2300A_XoscAacCode();

	CMT2300A_GoStby();

	/* If you enable antenna switch, GPIO1/GPIO2 will output RX_ACTIVE/TX_ACTIVE,
	   and it can't output INT1/INT2 via GPIO1/GPIO2 */
	CMT2300A_EnableAntennaSwitch(0);

	CMT2300A_ConfigGpio(CMT2300A_GPIO3_SEL_INT2);
	CMT2300A_SetInterruptPolar(true);

	if(CfgMode == eRadioCfgTxMode)
	{
		CMT2300A_ConfigInt2Interrupt(CMT2300A_INT_SEL_TX_DONE);
		CMT2300A_EnableInterrupt(CMT2300A_MASK_TX_DONE_EN);
	}
	else if(CfgMode == eRadioCfgRxMode)
	{
		CMT2300A_ConfigInt2Interrupt(CMT2300A_INT_SEL_PKT_DONE);
		CMT2300A_EnableInterrupt(CMT2300A_MASK_PKT_DONE_EN);
	}

	CMT2300A_EnableLfosc(true);

	CMT2300A_ClearInterruptFlags();

	if((CfgMode == eRadioCfgRxMode) || (CfgMode == eRadioCfgTestRxMode))
	{
		CMT2300A_GoStby();
		CMT2300A_ClearInterruptFlags();
		
		CMT2300A_EnableReadFifo();
		CMT2300A_ClearRxFifo();

		CMT2300A_GoRx();
	}
	else if(CfgMode == eRadioCfgTestFskTxMode)
	{
		CMT2300A_SetTxPrefix();
		CMT2300A_GoTx();
	}
	else if(CfgMode == eRadioCfgTestOokTxMode)
	{
		CMT2300A_GoTx();
	}
}

void RF_SetStatus(eRFStatus nStatus)
{
	g_nNextRFState = nStatus;
}

eRFStatus RF_GetStatus(void)
{
	return g_nNextRFState;
}

uint8_t RF_GetInterruptFlags(void)
{
	return g_nInterrutFlags;
}

extern bool SetRadioRxIntFlag(void);
extern bool ClrRadioRxIntFlag(void);

bool RF_Rx(uint8_t buf[], uint16_t len)
{
	bool RxRes = false;
	
	if(SetRadioRxIntFlag())
	{
		ClrRadioRxIntFlag();

		if(CMT2300A_MASK_PKT_OK_FLG & CMT2300A_ReadReg(CMT2300A_CUS_INT_FLAG))	/* Read PKT_OK flag */
		{
			CMT2300A_GoStby();
			
			CMT2300A_ReadFifo(buf, len);
			
			g_nInterrutFlags = CMT2300A_ClearInterruptFlags();
				
			CMT2300A_GoSleep();
			CMT2300A_GoRx();

			RxRes = true;
		}
	}
	return RxRes;
}

void RF_Send(uint8_t buf[], uint16_t len, uint16_t packnum, uint32_t timeout)
{
	uint8_t Reg = 0;
	
	g_pTxBuffer = buf;
	g_nTxLength = len;
	g_nTxTimeout = timeout;

	RF_Config(eRadioCfgTxMode);

	CMT2300A_GoStby();

	//CMT2300A_DutyCycle(false);
	Reg=CMT2300A_ReadReg(CMT2300A_CUS_SYS2);
	CMT2300A_WriteReg(CMT2300A_CUS_SYS2,Reg&0xE5);

	/* Must clear FIFO after enable SPI to read or write the FIFO */
	CMT2300A_EnableWriteFifo();
	CMT2300A_ClearTxFifo();

	//Reg=CMT2300A_ReadReg(CMT2300A_CUS_PKT29);                        
	//CMT2300A_WriteReg(CMT2300A_CUS_PKT29,Reg&0x7F);                  //¹Ø±Õrestore TX FIFO

	/* The length need be smaller than 32 */
	CMT2300A_WriteFifo(g_pTxBuffer, g_nTxLength);

	CMT2300A_ClearInterruptFlags();

	//Reg=CMT2300A_ReadReg(CMT2300A_CUS_PKT29);                        
	//CMT2300A_WriteReg(CMT2300A_CUS_PKT29,Reg|0x80);                  //´ò¿ªrestore TX FIFO

	if(0==(CMT2300A_MASK_TX_FIFO_NMTY_FLG & CMT2300A_ReadReg(CMT2300A_CUS_FIFO_FLAG)))
	{
		g_nNextRFState = RF_STATE_ERROR;
	}
	
	if(FALSE==CMT2300A_GoTx())
		g_nNextRFState = RF_STATE_ERROR;
	else
		g_nNextRFState = RF_STATE_TX_WAIT;
	
	g_nTxTimeCount = CMT2300A_GetTickCount();
}


void RF_WaitSendDone(void)
{
	uint16_t i = 0;
	eRFResult RfRes = RF_IDLE;

	for(i=0; i<SEND_MSG_TIMEOUT; i++)
	{
		HalDelayMs(1);
		RfRes = RF_Process();
		if(RfRes == RF_TX_DONE)
		{
			break;
		}
	}
}

eRFResult RF_Process(void)
{
	eRFResult nRes = RF_BUSY;

	switch(g_nNextRFState) 
	{
		case RF_STATE_IDLE:
		{
			nRes = RF_IDLE;
			break;
		}

		case RF_STATE_RX_START:
		{
			CMT2300A_GoStby();
			CMT2300A_ClearInterruptFlags();

			/* Must clear FIFO after enable SPI to read or write the FIFO */
			CMT2300A_EnableReadFifo();
			CMT2300A_ClearRxFifo();

			if(FALSE==CMT2300A_GoRx())
				g_nNextRFState = RF_STATE_ERROR;
			else
				g_nNextRFState = RF_STATE_RX_WAIT;

			g_nRxTimeCount = CMT2300A_GetTickCount();

			break;
		}

		case RF_STATE_RX_WAIT:
		{
#ifdef ENABLE_ANTENNA_SWITCH
			if(CMT2300A_MASK_PKT_OK_FLG & CMT2300A_ReadReg(CMT2300A_CUS_INT_FLAG))  /* Read PKT_OK flag */
#else
			if(CMT2300A_ReadGpio2())  /* Read INT2, PKT_OK */
#endif
			{
				g_nNextRFState = RF_STATE_RX_DONE;
			}

			if((INFINITE != g_nRxTimeout) && ((CMT2300A_GetTickCount()-g_nRxTimeCount) > g_nRxTimeout))
			{
				g_nNextRFState = RF_STATE_RX_TIMEOUT;
			}
			break;
		}

		case RF_STATE_RX_DONE:
		{
			CMT2300A_GoStby();

			/* The length need be smaller than 32 */
			CMT2300A_ReadFifo(g_pRxBuffer, g_nRxLength);

			g_nInterrutFlags = CMT2300A_ClearInterruptFlags();
			    
			CMT2300A_GoSleep();

			g_nNextRFState = RF_STATE_IDLE;
			nRes = RF_RX_DONE;
			break;
		}

		case RF_STATE_RX_TIMEOUT:
		{
			CMT2300A_GoSleep();

			g_nNextRFState = RF_STATE_IDLE;
			nRes = RF_RX_TIMEOUT;
			break;
		}
	
		case RF_STATE_TX_START:
		{
			CMT2300A_GoStby();
			CMT2300A_ClearInterruptFlags();

			/* Must clear FIFO after enable SPI to read or write the FIFO */
			CMT2300A_EnableWriteFifo();
			CMT2300A_ClearTxFifo();

			/* The length need be smaller than 32 */
			CMT2300A_WriteFifo(g_pTxBuffer, g_nTxLength);

			if(0==(CMT2300A_MASK_TX_FIFO_NMTY_FLG & CMT2300A_ReadReg(CMT2300A_CUS_FIFO_FLAG)))
			{
				g_nNextRFState = RF_STATE_ERROR;
			}

			if(FALSE==CMT2300A_GoTx())
				g_nNextRFState = RF_STATE_ERROR;
			else
				g_nNextRFState = RF_STATE_TX_WAIT;

			g_nTxTimeCount = CMT2300A_GetTickCount();
			break;
		}

		case RF_STATE_TX_WAIT:
		{
#ifdef ENABLE_ANTENNA_SWITCH
			if(CMT2300A_MASK_TX_DONE_FLG & CMT2300A_ReadReg(CMT2300A_CUS_INT_CLR1))  /* Read TX_DONE flag */
#else
			if(CMT2300A_ReadGpio1())  /* Read INT1, TX_DONE */
#endif
			{
				g_nNextRFState = RF_STATE_TX_DONE;
			}

			if((INFINITE != g_nTxTimeout) && ((CMT2300A_GetTickCount()-g_nTxTimeCount) > g_nTxTimeout))
			{
				g_nNextRFState = RF_STATE_TX_TIMEOUT;
			}
			break;
		}

		case RF_STATE_TX_DONE:
		{
			if(Radio.InitMode == LowPowerHubMode)
			{
				CMT2300Init(NormalHubMode);
			}
			else if(Radio.InitMode == LowPowerIntranetMode)
			{
				CMT2300Init(NormalIntranetMode);
			}
			else
			{
				RF_Config(eRadioCfgRxMode);
			}
			
			g_nNextRFState = RF_STATE_IDLE;
			nRes = RF_TX_DONE;
			break;
		}

		case RF_STATE_TX_TIMEOUT:
		{
			CMT2300A_GoSleep();

			g_nNextRFState = RF_STATE_IDLE;
			nRes = RF_TX_TIMEOUT;
			break;
		}

		case RF_STATE_ERROR:
		{
			CMT2300A_SoftReset();
			CMT2300A_DelayMs(20);

			RF_Config(eRadioCfgRxMode);

			g_nNextRFState = RF_STATE_IDLE;
			nRes = RF_ERROR;
			break;
		}

		default:
			break;
	}

	return nRes;
}


