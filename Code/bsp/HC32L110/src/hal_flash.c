#include "../inc/hal_flash.h"
#include "ddl.h"

#define FLASH_END_MAX_ADDR          (0x00007FFFu)

/**********************************************************************/
//Description:	hal_flash_init()										
//Parameters:                        									
//Return:   															
//Date:           
//Author:		quanwu.xu  
/**********************************************************************/
void hal_flash_init(void)
{
	Flash_SectorErase(0x00007E00u);
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
	if(FLASH_END_MAX_ADDR < addr)
	{
		return;
	}

	Flash_WriteWord(addr, Data);
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

