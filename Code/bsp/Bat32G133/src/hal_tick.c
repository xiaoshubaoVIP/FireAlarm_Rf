#include "../inc/hal_tick.h"
#include "BAT32G133.h"

uint32_t SysDelayTime = 0;
uint32_t HalGetTickMs(void);


/**********************************************************************/
//Description:	HalDelayUs()											
//Parameters:                        									
//Return:   															
//Date:           
//Author:		quanwu.xu  
/**********************************************************************/
void HalDelayUs(uint32_t u32Us)
{
	for(;u32Us!=0;u32Us--)
	{
		__NOP();
	}
}


/**********************************************************************/
//Description:	HalDelayMs()											
//Parameters:                        									
//Return:   															
//Date:           
//Author:		quanwu.xu  
/**********************************************************************/
void HalDelayMs(uint32_t u32Ms)
{
#if 0
	for(SysDelayTime=0; SysDelayTime<u32Ms;)
	{
		HalDelayUs(1);
	}
#else		//Optimaiztion Level-2, MCU system halted;
	uint32_t u32TimeMsOld;    // 开始时间
	uint32_t u32TimeMsNew;    // 当前时间
	uint32_t u32TimeMsDiff;   // 时间差值
	if (0 != u32Ms)
	{
		u32TimeMsOld  = HalGetTickMs();
		do
		{
			u32TimeMsNew  = HalGetTickMs();
			u32TimeMsDiff = u32TimeMsNew - u32TimeMsOld;
			HalDelayUs(1);
		}while(u32TimeMsDiff < u32Ms);
	}
#endif
}

/**********************************************************************/
//Description:	SysTick_Handler()												
//Parameters:                        									
//Return:   															
//Date:           
//Author:		quanwu.xu  
/**********************************************************************/
extern void SysTickAdd(void);
void SysTick_Handler(void)
{
	SysTickAdd();
	SysDelayTime++;
}

/**********************************************************************/
//Description:	HalGetTickMs()												
//Parameters:                        									
//Return:   															
//Date:           
//Author:		quanwu.xu  
/**********************************************************************/
uint32_t HalGetTickMs(void)
{
	return SysDelayTime;
}


/**********************************************************************/
//Description:	SysTickInit()												
//Parameters:                        									
//Return:   															
//Date:           
//Author:		quanwu.xu  
/**********************************************************************/
void HalTickInit(void)
{
	SysTick_Config(SystemCoreClock / 1000); 
}

