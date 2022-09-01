#ifndef __HAL_FLASH_H__
#define __HAL_FLASH_H__

#include "base_types.h"

/* 关闭日志打印，只本模块 */
#define UNUSED_LOG

void hal_flash_init(void);
void hal_flash_write(uint32_t addr, uint32_t Data);
void hal_flash_read(uint32_t* ReadBuf, uint32_t u32Addr);

#endif
