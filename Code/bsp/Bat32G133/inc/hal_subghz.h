#ifndef __HAL_SUBGHZ_H__
#define __HAL_SUBGHZ_H__

#include <stdint.h>

void CMT2300A_InitGpio(void);

uint8_t CMT2300A_ReadReg(uint8_t addr);
void CMT2300A_WriteReg(uint8_t addr, uint8_t dat);

void CMT2300A_ReadFifo(uint8_t buf[], uint16_t len);
void CMT2300A_WriteFifo(const uint8_t buf[], uint16_t len);

#endif
