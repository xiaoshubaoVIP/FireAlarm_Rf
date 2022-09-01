#include "bsp_config.h"
#include "common.h"
#include "gpio.h"
#include "BAT32G133.h"

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
	SystemCoreClockUpdate();
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
	PORT_Init(PORT0, PIN0, OPENDRAIN_OUTPUT);
	PORT_Init(PORT0, PIN1, OPENDRAIN_OUTPUT);
	PORT_Init(PORT0, PIN2, OPENDRAIN_OUTPUT);

	PORT_ClrBit(PORT0, PIN0);
	PORT_ClrBit(PORT0, PIN1);
	PORT_ClrBit(PORT0, PIN2);

	PORT_Init(PORT1, PIN0, OPENDRAIN_OUTPUT);
	PORT_Init(PORT1, PIN1, OPENDRAIN_OUTPUT);
	PORT_Init(PORT1, PIN2, OPENDRAIN_OUTPUT);
	PORT_Init(PORT1, PIN3, OPENDRAIN_OUTPUT);

	PORT_ClrBit(PORT1, PIN0);
	PORT_ClrBit(PORT1, PIN1);
	PORT_ClrBit(PORT1, PIN2);
	PORT_ClrBit(PORT1, PIN3);

	PORT_Init(PORT2, PIN0, OPENDRAIN_OUTPUT);
	PORT_Init(PORT2, PIN1, OPENDRAIN_OUTPUT);
	PORT_Init(PORT2, PIN4, OPENDRAIN_OUTPUT);
	PORT_Init(PORT2, PIN5, OPENDRAIN_OUTPUT);
	PORT_Init(PORT2, PIN6, OPENDRAIN_OUTPUT);

	PORT_ClrBit(PORT2, PIN0);
	PORT_ClrBit(PORT2, PIN1);
	PORT_ClrBit(PORT2, PIN4);
	PORT_ClrBit(PORT2, PIN5);
	PORT_ClrBit(PORT2, PIN6);

	PORT_Init(PORT3, PIN0, OPENDRAIN_OUTPUT);
	PORT_Init(PORT3, PIN1, OPENDRAIN_OUTPUT);
	PORT_Init(PORT3, PIN2, OPENDRAIN_OUTPUT);
	PORT_Init(PORT3, PIN5, OPENDRAIN_OUTPUT);
	PORT_Init(PORT3, PIN6, OPENDRAIN_OUTPUT);
	PORT_Init(PORT3, PIN7, OPENDRAIN_OUTPUT);

	PORT_ClrBit(PORT3, PIN0);
	PORT_ClrBit(PORT3, PIN1);
	PORT_ClrBit(PORT3, PIN2);
	PORT_ClrBit(PORT3, PIN5);
	PORT_ClrBit(PORT3, PIN6);
	PORT_ClrBit(PORT3, PIN7);
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
	HalRtcInit();
	HalTickInit();
}


