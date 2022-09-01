#ifndef __HAL_TIM_H__
#define __HAL_TIM_H__

#include <stdint.h>

typedef enum {
	TIMER_0 = 0,
	TIMER_1,
	TIMER_2,
	TIMER_3 ,
} tim_type;

void HalTimInit(tim_type Dev);
uint16_t HalGetTimCnt(tim_type Dev);

#endif
