#ifndef __EVENT_H__
#define __EVENT_H__

typedef enum {
    EVENT_UART_RX,
    EVENT_TIM_TIMEOUT,
    EVENT_RTC_DATE,
} hw_event_t;


typedef unsigned long FLAG;

typedef struct event_t* p_event;



#endif
