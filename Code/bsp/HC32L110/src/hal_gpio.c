#include "../inc/hal_gpio.h"
#include "ddl.h"

#ifdef C99_MODE

static const struct
{
	uint8_t port;
	uint8_t pin;
}	gpio_describe[] = {
	[PIN_KEY] = {0x02, 0X07},
	[PIN_RF_INT] = {0x02, 0X03},
	[PIN_UART_INT] = {0x03, 0X06},
	[PIN_RADIO_GPIO_1] = {0x00, 0X00},
	[PIN_RADIO_GPIO_2] = {0x00, 0X00},
	[PIN_RADIO_GPIO_3] = {0x00, 0X00},
};
	
#else

typedef struct
{
	uint8_t port;
	uint8_t pin;
}StrGpio;

StrGpio gpio_describe[] = {
	{0x02, 0X07},
	{0x02, 0X03},
	{0x03, 0X06},
	{0x00, 0X00},
	{0x00, 0x00},
	{0x00, 0x00},
};
#endif

/**********************************************************************/
//Description:		Gpio_IRQHandler()													  
//Parameters:       
//                        												  
//Return:   
//Date			2019-01-17
//Author:		quanwu.xu
/**********************************************************************/
extern bool RadioRxIntflag;
extern bool UartRxIntflag;

void Gpio_IRQHandler(uint8_t u8Param)
{
	//P2.3
	if(gpio_describe[PIN_RF_INT].port == u8Param)
	{
		if(TRUE == Gpio_GetIrqStat(gpio_describe[PIN_RF_INT].port, gpio_describe[PIN_RF_INT].pin))
		{	
			Gpio_ClearIrq(gpio_describe[PIN_RF_INT].port, gpio_describe[PIN_RF_INT].pin);
			RadioRxIntflag = true;
		}
	}

	//P3.6	
	if(gpio_describe[PIN_UART_INT].port == u8Param)
	{
		if(TRUE == Gpio_GetIrqStat(gpio_describe[PIN_UART_INT].port, gpio_describe[PIN_UART_INT].pin))
		{
			Gpio_ClearIrq(gpio_describe[PIN_UART_INT].port, gpio_describe[PIN_UART_INT].pin);
			UartRxIntflag = true;
		}
	}

	//P2.7
	if(gpio_describe[PIN_KEY].port == u8Param)
	{
		if(TRUE == Gpio_GetIrqStat(gpio_describe[PIN_KEY].port, gpio_describe[PIN_KEY].pin))
		{
			Gpio_ClearIrq(gpio_describe[PIN_KEY].port, gpio_describe[PIN_KEY].pin);		
		}
	}
}

/**********************************************************************/
//Description:	HalGpioSetMode()												
//Parameters:                        									
//Return:   															
//Date:           
//Author:		quanwu.xu  
/**********************************************************************/
void HalGpioSetMode(pin_id_t pin, pin_mode_t mode)
{
	if(mode == PIN_MODE_OUTPUT)
	{
		Gpio_InitIO(gpio_describe[pin].port, gpio_describe[pin].pin, GpioDirOut);
	}
	else if(mode == PIN_MODE_INPUT)
	{
		Gpio_InitIO(gpio_describe[pin].port, gpio_describe[pin].pin, GpioDirIn);
	}
	else if(mode == PIN_MODE_OUTPUT_OD)
	{
		Gpio_InitIOExt(gpio_describe[pin].port, gpio_describe[pin].pin, GpioDirOut,
														FALSE, FALSE, TRUE, FALSE);
	}
	else if(mode == PIN_MODE_OUTPUT_PULLPU)
	{
		Gpio_InitIOExt(gpio_describe[pin].port, gpio_describe[pin].pin, GpioDirOut,
														TRUE, FALSE, FALSE, FALSE);
	}
	else if(mode == PIN_MODE_INPUT_PULLUP)
	{
		Gpio_InitIOExt(gpio_describe[pin].port, gpio_describe[pin].pin, GpioDirIn,
														TRUE, FALSE, FALSE, FALSE);
	}
	else if(mode == PIN_MODE_INPUT_PULLDOWN)
	{
		Gpio_InitIOExt(gpio_describe[pin].port, gpio_describe[pin].pin, GpioDirIn,
														FALSE, TRUE, FALSE, FALSE);
	}
}


/**********************************************************************/
//Description:	HalGpioSetIRQMode()												
//Parameters:                        									
//Return:   															
//Date:           
//Author:		quanwu.xu  
/**********************************************************************/
void HalGpioSetIRQMode(pin_id_t pin, pin_Irq_mode_t irq_mode)
{
	if (irq_mode == PIN_IRQ_RISING) {
		Gpio_EnableIrq(gpio_describe[pin].port, gpio_describe[pin].pin, GpioIrqRising);
		EnableNvic(gpio_describe[pin].port, DDL_IRQ_LEVEL_DEFAULT, TRUE);
	}
	else if (irq_mode == PIN_IRQ_FALLING){
		Gpio_EnableIrq(gpio_describe[pin].port, gpio_describe[pin].pin, GpioIrqFalling);
		EnableNvic(gpio_describe[pin].port, DDL_IRQ_LEVEL_DEFAULT, TRUE);
	}
}

/**********************************************************************/
//Description:	HalGpioSet()											
//Parameters:                        									
//Return:   															
//Date:           
//Author:		quanwu.xu  
/**********************************************************************/
void HalGpioSet(pin_id_t pin, pin_status_t status)
{
	if (status == PIN_SET) {
		Gpio_SetIO(gpio_describe[pin].port, gpio_describe[pin].pin, 1);
	}
	else if (status == PIN_RESET){
		Gpio_SetIO(gpio_describe[pin].port, gpio_describe[pin].pin, 0);
	}
}

/**********************************************************************/
//Description:	HalGpioRead()												
//Parameters:                        									
//Return:   															
//Date:           
//Author:		quanwu.xu  
/**********************************************************************/
pin_status_t HalGpioRead(pin_id_t pin)
{
	if(Gpio_GetIO(gpio_describe[pin].port, gpio_describe[pin].pin) == 1u)
	{
		return PIN_SET;
	}
	else
	{
		return PIN_RESET;
	}
}

