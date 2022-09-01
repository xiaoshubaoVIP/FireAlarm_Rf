#include "../inc/hal_gpio.h"
#include "gpio.h"
#include "intp.h"

#ifndef C99_MODE

typedef struct
{
	PORT_TypeDef port;
	PIN_TypeDef pin;
	uint8_t IntPinMsk;
}StrGpio;

StrGpio gpio_describe[] = {
	{PORT2, PIN0, 0x08},
	{PORT3, PIN7, 0x02},
	{PORT2, PIN6, 0x04},
	{PORT0, PIN0, 0x00},
	{PORT0, PIN0, 0x00},
	{PORT0, PIN0, 0x00},
};

#else

static const struct
{
	uint8_t port;
	uint8_t pin;
}	gpio_describe[] = {
	[PIN_KEY] = {0x02, 0X00},
	[PIN_RF_INT] = {0x03, 0X07},
	[PIN_UART_INT] = {0x02, 0X06},
	[PIN_RADIO_GPIO_1] = {0x00, 0X00},
	[PIN_RADIO_GPIO_2] = {0x00, 0X00},
	[PIN_RADIO_GPIO_3] = {0x00, 0X00},
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
void Gpio_IRQHandler(uint8_t u8Param)
{
	
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
		PORT_Init(gpio_describe[pin].port, gpio_describe[pin].pin, OUTPUT);
	}
	else if(mode == PIN_MODE_INPUT)
	{
		PORT_Init(gpio_describe[pin].port, gpio_describe[pin].pin, INPUT);
	}
	else if(mode == PIN_MODE_OUTPUT_OD)
	{
		PORT_Init(gpio_describe[pin].port, gpio_describe[pin].pin, OPENDRAIN_OUTPUT);
	}
	else if(mode == PIN_MODE_INPUT_PULLUP)
	{
		PORT_Init(gpio_describe[pin].port, gpio_describe[pin].pin, PULLUP_INPUT);
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
	if (irq_mode == PIN_IRQ_RISING)
	{
		//PORT_Init(gpio_describe[pin].port, gpio_describe[pin].pin, PULLUP_INPUT);
		INTP_Init(gpio_describe[pin].IntPinMsk, INTP_RISING);
		INTP_Start(gpio_describe[pin].IntPinMsk);	
	}
	else if (irq_mode == PIN_IRQ_FALLING)
	{
		//PORT_Init(gpio_describe[pin].port, gpio_describe[pin].pin, PULLUP_INPUT);
		INTP_Init(gpio_describe[pin].IntPinMsk, INTP_FALLING);
		INTP_Start(gpio_describe[pin].IntPinMsk);	
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
	INTP_Stop(gpio_describe[pin].IntPinMsk);
	if (status == PIN_SET) {
		PORT_SetBit(gpio_describe[pin].port, gpio_describe[pin].pin);
	}
	else if (status == PIN_RESET){
		PORT_ClrBit(gpio_describe[pin].port, gpio_describe[pin].pin);
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
	if(PORT_GetBit(gpio_describe[pin].port, gpio_describe[pin].pin) == 0u)
	{
		return PIN_RESET;
	}
	else
	{
		return PIN_SET;
	}

}

