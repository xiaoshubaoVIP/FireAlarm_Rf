#include "bsp_config.h"
#include "ddl.h"
#include "common.h"

/**********************************************************************/
//Description:	McuStop()											
//Parameters:                        									
//Return:   															
//Date:           
//Author:		quanwu.xu  
/**********************************************************************/
void McuStop(void)
{
	__STOP();
}

/**********************************************************************/
//Description:	SoftwareReset()											
//Parameters:                        									
//Return:   															
//Date:           
//Author:		quanwu.xu  
/**********************************************************************/
void SoftwareReset(void)
{
	NVIC_SystemReset();
}

/**********************************************************************/
//Description:	ClockInit()										
//Parameters:                        									
//Return:   															
//Date:           
//Author:		quanwu.xu  
/**********************************************************************/
static void ClockInit(void)
{
	Clk_SetRCHFreq(ClkFreq16Mhz);	    //16M
	Clk_SetHClkDiv(ClkDiv1);			//CPU时钟
	Clk_SetPClkDiv(ClkDiv1);			//外设时钟
}

/**********************************************************************/
//Description:	GpioInit()										
//Parameters:                        									
//Return:   															
//Date:           
//Author:		quanwu.xu  
/**********************************************************************/
static void GpioInit(void)
{
	Clk_SetPeripheralGate(ClkPeripheralGpio,TRUE);	//开启时钟
	M0P_GPIO->P0DIR=0x00000000;
	M0P_GPIO->P1DIR=0x00000000;
	M0P_GPIO->P2DIR=0x00000000;
	M0P_GPIO->P3DIR=0x00000000;

	M0P_GPIO->P0OUT=0x00000000;
	M0P_GPIO->P1OUT=0x00000000;
	M0P_GPIO->P2OUT=0x00000000;
	M0P_GPIO->P3OUT=0x00000000;

	M0P_GPIO->P0PU=0x00000000;
	M0P_GPIO->P1PU=0x00000000;
	M0P_GPIO->P2PU=0x00000000;
	M0P_GPIO->P3PU=0x00000000;
}


/**********************************************************************/
//Description:	BoardInit()											
//Parameters:                        									
//Return:   															
//Date:           
//Author:		quanwu.xu  
/**********************************************************************/
void BoardInit(void)
{
	ClockInit();
	GpioInit();

	HalUartInit(UART_NET, 9600u);
	HalTimInit(TIMER_1);
	//HalRtcInit();
	HalTickInit();
}

