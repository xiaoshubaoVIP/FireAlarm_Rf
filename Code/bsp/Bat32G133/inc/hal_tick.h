#ifndef __HAL_TICK_H__
#define __HAL_TICK_H__

#include <stdint.h>

extern uint32_t SysDelayTime;

uint32_t HalGetTickMs(void);
void HalDelayUs(uint32_t u32Us);
void HalDelayMs(uint32_t u32Ms);
void HalTickInit(void);
#endif
