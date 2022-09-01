#include "debug.h"
#include "system.h"
#include "bsp_config.h"

static unsigned long SysActiveFlag;

SystemApiTag SystemApiTagObj;


/**********************************************************************/
//Description:  SysTickAdd()											
//Parameters:                        									
//Return:   															
//Date:         2019-01-08
//Author:		quanwu.xu  
/**********************************************************************/
eSystemState  GetSystemState(void)
{
	return SystemApiTagObj.SystemState;
}


/**********************************************************************/
//Description:  SysTickAdd()											
//Parameters:                        									
//Return:   															
//Date:         2019-01-08
//Author:		quanwu.xu  
/**********************************************************************/
void SysTickAdd(void)
{
	SystemApiTagObj.SystemTick++;
}

/**********************************************************************/
//Description:  SysGetTick()											
//Parameters:                        									
//Return:   															
//Date:         2019-01-08
//Author:		quanwu.xu  
/**********************************************************************/
uint32_t SysGetTick(void)
{
	return SystemApiTagObj.SystemTick;
}


/**********************************************************************/
//Description:	SetSysSleepFlag()											
//Parameters:                        										
//Return:   																
//Date:           
//Author:	quanwu.xu  
/**********************************************************************/
void SetSysSleepFlag(eSysActiveFlag Flag)
{
	SysActiveFlag |= Flag;
}

/**********************************************************************/
//Description:	ResetSysSleepFlag()											
//Parameters:                        										
//Return:   																
//Date:           
//Author:	quanwu.xu  
/**********************************************************************/
void ResetSysSleepFlag(eSysActiveFlag Flag)
{
	SysActiveFlag &= ~Flag;
}


/**********************************************************************/
//Description:	GetSysSleepFlag()										
//Parameters:                        									
//Return:   															
//Date:           
//Author:	quanwu.xu  
/**********************************************************************/
uint32_t GetSysSleepFlag(void)
{
	return SysActiveFlag;
}


/**********************************************************************/
//Description:		GetCrc8()											
//Parameters:       
//                        												
//Return:   
//Date			2019-01-17
//Author:		quanwu.xu
/**********************************************************************/
uint8_t GetCrc8(uint8_t *buf, uint8_t len)
{
	int i;
	char crc = 0;
	while(len--!=0)
	{
		for (i = 0x80; i != 0; i /= 2)
		{
			if ((crc & 0x80) != 0)
			{
				crc *= 2;
				crc ^= 0x07;
			} 
			else
			crc *= 2;

			if ((*buf & i) != 0)
			crc ^= 0x07;
		}
		buf++;
	}
	return crc;
}


/**********************************************************************/
//Name:			GetRandDelayTime()
//Description:	Get a rand number (2 ~ 20)*20ms										  
//Parameters:                        												  
//Return:   																  
//Date:           
//Author:		quanwu.xu  
/**********************************************************************/
uint16_t GetRandDelayTime(eDelayMode DelayMode)
{
	uint16_t Seed = 0;
	uint16_t RandValue1, RandValue2 = 0;
	uint16_t RandValue = 0;
	
	//Seed = Bt_Cnt16Get(TIM0);
	//Delay(Seed);
	Seed = HalGetTimCnt(TIMER_1);
	srand(Seed);

	if(DelayMode == eLowpowerTimeMode)
	{
		RandValue = (rand()%35+5)*DELAY_SEND_MSG_INTERVAL_TIME + TX_DELAY_LONG_TIME;	
	}
	else if(DelayMode == eNormalTimeMode)
	{
		RandValue = (rand()%50+5)*POLLING_SEND_MSG_INTERVAL_TIME + 2000;				//25*(5~50)
	}
	else if(DelayMode == eLowpowerIntranetMode)
	{
		RandValue1 = rand()%400+1;
		HalDelayUs(Seed);
		Seed =  HalGetTimCnt(TIMER_1);
		RandValue2 = rand()%300+1;	
	
		RandValue = (RandValue1*5) + (RandValue2*10) ;
	}
	else if(DelayMode == eNormalIntranetMode)
	{
		RandValue = (rand()%200+1)*5;
	}

	debug_printf("T' = %d\n", RandValue);
	return RandValue;
}

