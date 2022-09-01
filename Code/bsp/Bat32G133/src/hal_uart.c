#include "../inc/hal_uart.h"
#include "sci.h"

extern void UartReceiveInput(unsigned char value);


/**********************************************************************/
//Description:	RxIntCallback()											
//Parameters:                        									
//Return:   															
//Date:           
//Author:		quanwu.xu  
/**********************************************************************/
void RxIntCallback(void)
{

}


void ErrIntCallback(void)
{
	
}

/**********************************************************************/
//Description:	UartInit()												
//Parameters:                        									
//Return:   															
//Date:           
//Author:		quanwu.xu  
/**********************************************************************/
extern uint32_t SystemCoreClock;
void HalUartInit(uart_type dev, unsigned long bound)
{
	uint32_t SysClk = SystemCoreClock;

	if(UART0_Init(SysClk, bound) == MD_ERROR)
	{
		while(1);
	}
}

/**********************************************************************/
//Description:	HalUartWrite()												
//Parameters:                        									
//Return:   															
//Date:           
//Author:		quanwu.xu  
/**********************************************************************/
void HalUartWrite(uart_type dev, uint8_t *SendBuf, uint8_t len)
{
	unsigned short i;

	for (i = 0; i < len; i++) {
		UART0_Send(SendBuf[i]);
	}
}

/**********************************************************************/
//Description:	HalUartRead()												
//Parameters:                        									
//Return:   															
//Date:           
//Author:		quanwu.xu  
/**********************************************************************/
void HalUartRead(uart_type dev, uint8_t *ReadBuf, uint8_t len)
{

}

