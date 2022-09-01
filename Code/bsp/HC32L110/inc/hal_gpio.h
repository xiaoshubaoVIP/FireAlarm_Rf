#ifndef __HAL_GPIO_H__
#define __HAL_GPIO_H__

typedef enum
{
	PIN_KEY = 0,
	PIN_RF_INT,
	PIN_UART_INT,
	PIN_RADIO_GPIO_1,
	PIN_RADIO_GPIO_2,
	PIN_RADIO_GPIO_3,
} pin_id_t;


typedef enum
{
	PIN_MODE_OUTPUT = 0,
	PIN_MODE_INPUT,
	PIN_MODE_OUTPUT_OD,
	PIN_MODE_OUTPUT_PULLPU,
	PIN_MODE_INPUT_PULLUP, 
	PIN_MODE_INPUT_PULLDOWN,
} pin_mode_t;

typedef enum
{
	PIN_IRQ_HIHG = 0,
	PIN_IRQ_LOW,
	PIN_IRQ_RISING,
	PIN_IRQ_FALLING,
} pin_Irq_mode_t;


typedef enum
{
	PIN_RESET = 0,
	PIN_SET,
} pin_status_t;

void HalGpioSetMode(pin_id_t pin, pin_mode_t mode);
void HalGpioSetIRQMode(pin_id_t pin, pin_Irq_mode_t irq_mode);
void HalGpioSet(pin_id_t pin, pin_status_t status);
pin_status_t HalGpioRead(pin_id_t pin);

#endif
