#ifndef __HAL_UART_H__
#define __HAL_UART_H__

#include "base_types.h"

typedef enum {
	UART_NET,   /* ģ��ͨѶ */
} uart_type;

void HalUartInit(uart_type dev, unsigned long bound);
void HalUartWrite(uart_type dev, uint8_t *SendBuf, uint8_t len);
void HalUartRead(uart_type dev, uint8_t *ReadBuf, uint8_t len);

#endif
