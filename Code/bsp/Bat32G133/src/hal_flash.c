#include "../inc/hal_flash.h"
#include "flash.h"

#define FLASH_END_MAX_ADDR          (0x00500FFFu)

/**********************************************************************/
//Description:	hal_flash_init()										
//Parameters:                        									
//Return:   															
//Date:           
//Author:		quanwu.xu  
/**********************************************************************/
void hal_flash_init(void)
{
	EraseSector(0x00500100u);
}

/**********************************************************************/
//Description:	hal_flash_write()										
//Parameters:                        									
//Return:   															
//Date:           
//Author:		quanwu.xu  
/**********************************************************************/
void hal_flash_write(uint32_t addr, uint32_t Data)
{
	uint8_t i = 0;
	uint8_t Buf[4] = {0};
	for(i=0; i<4; i++)
	{
		Buf[i] = (Data>>(i*8) & 0xFF);
	}

	if(FLASH_END_MAX_ADDR < addr)
	{
		return;
	}

	flash_write(addr, 4, (uint8_t *)&Buf);
}


/**********************************************************************/
//Description:	hal_flash_read()										
//Parameters:                        									
//Return:   															
//Date:           
//Author:		quanwu.xu  
/**********************************************************************/
void hal_flash_read(uint32_t* ReadBuf, uint32_t u32Addr)
{
	if((FLASH_END_MAX_ADDR < u32Addr) || (ReadBuf == NULL))
	{
		return;
	}

	*ReadBuf = *((volatile uint32_t*)u32Addr);	
}

