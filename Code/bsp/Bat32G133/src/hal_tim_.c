#include "../inc/hal_tim.h"
#include "tim4.h"

/**********************************************************************/
//Description:	HalGetTimCnt()											
//Parameters:                        									
//Return:   															
//Date:           
//Author:		quanwu.xu  
/**********************************************************************/
uint16_t HalGetTimCnt(tim_type Dev)
{
	return 0;
}

/**********************************************************************/
//Description:	Time1IrqCb()											
//Parameters:                        									
//Return:   															
//Date:           
//Author:		quanwu.xu  
/**********************************************************************/
void Time1IrqCb(void)
{

}

/**********************************************************************/
//Description:	Time1Init()												
//Parameters:                        									
//Return:   															
//Date:           
//Author:		quanwu.xu  
/**********************************************************************/
void HalTimInit(tim_type Dev)
{
	TM40_IntervalTimer(TM4_CHANNEL_0, 20);

	TM41_IntervalTimer(TM4_CHANNEL_0, 65535);
}


