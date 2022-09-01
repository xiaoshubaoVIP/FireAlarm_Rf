#include "../inc/hal_uart.h"
#include "ddl.h"

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
	uint8_t RxChar = M0P_UART0->SBUF;
	UartReceiveInput(RxChar);
	Uart_ClrStatus(UARTCH0,UartRxFull);
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
void HalUartInit(uart_type dev, unsigned long bound)
{
	uint16_t timer=0;
	uint32_t pclk=0;

	stc_uart_config_t  stcConfig;
	stc_uart_irq_cb_t stcUartIrqCb;
	stc_uart_multimode_t stcMulti;
	stc_uart_baud_config_t stcBaud;
	stc_bt_config_t stcBtConfig;


	DDL_ZERO_STRUCT(stcUartIrqCb);
	DDL_ZERO_STRUCT(stcMulti);
	DDL_ZERO_STRUCT(stcBaud);
	DDL_ZERO_STRUCT(stcBtConfig);


	Gpio_InitIOExt(0,1,GpioDirOut,TRUE,FALSE,FALSE,FALSE);   
	Gpio_InitIOExt(0,2,GpioDirOut,TRUE,FALSE,FALSE,FALSE); 

	//通道端口配置
	Gpio_SetFunc_UART0_RXD_P01();
	Gpio_SetFunc_UART0_TXD_P02();

	//外设时钟使能
	Clk_SetPeripheralGate(ClkPeripheralBt,TRUE);//模式0/2可以不使�?
	Clk_SetPeripheralGate(ClkPeripheralUart0,TRUE);

	stcUartIrqCb.pfnRxIrqCb = RxIntCallback;
	stcUartIrqCb.pfnTxIrqCb = NULL;
	stcUartIrqCb.pfnRxErrIrqCb = ErrIntCallback;
	stcConfig.pstcIrqCb = &stcUartIrqCb;
	stcConfig.bTouchNvic = TRUE;


	stcConfig.enRunMode = UartMode1;//测试项，更改此处来转�?种模式测�?


	stcMulti.enMulti_mode = UartNormal;//测试项，更改此处来转换多主机模式，mode2/3才有多主机模�?

	stcConfig.pstcMultiMode = &stcMulti;

	stcBaud.bDbaud = 0u;//双倍波特率功能
	stcBaud.u32Baud = bound;//更新波特率位�?
	stcBaud.u8Mode = UartMode1; //计算波特率需要模式参�?
	pclk = Clk_GetPClkFreq();
	timer=Uart_SetBaudRate(UARTCH0,pclk,&stcBaud);

	stcBtConfig.enMD = BtMode2;
	stcBtConfig.enCT = BtTimer;
	Bt_Init(TIM0, &stcBtConfig);//调用basetimer1设置函数产生波特�?
	Bt_ARRSet(TIM0,timer);
	Bt_Cnt16Set(TIM0,timer);
	Bt_Run(TIM0);

	Uart_Init(UARTCH0, &stcConfig);
	Uart_EnableIrq(UARTCH0,UartRxIrq);
	Uart_ClrStatus(UARTCH0,UartRxFull);
	Uart_EnableFunc(UARTCH0,UartRx);


	Uart_SetMMDOrCk(UARTCH0,Data);
	Uart_ClrStatus(UARTCH0,UartTxEmpty); 
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
		Uart_SendData(UARTCH0, SendBuf[i]);
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

