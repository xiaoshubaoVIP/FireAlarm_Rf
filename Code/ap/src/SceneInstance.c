#include "SceneInstance.h"
#include "common.h"


#define SYSTEMTICKS                   ((uint32_t)0xffffffff)

static TmrHeader gTmrHeader;


static uint32_t GetTicks(void)
{
	uint32_t Ret = 0;
	//to be done
	static uint32_t lastTick10ms = 0;
	uint32_t  tick = SysGetTick(); 

	uint32_t  dTick = (tick >= lastTick10ms) ? (tick - lastTick10ms) : (SYSTEMTICKS - lastTick10ms + tick);
	if (dTick < 1)
	{
		Ret = 0;
	}
	else
	{	//10ms
		Ret = dTick;
		lastTick10ms = tick;
	}

	//to be done
	return (Ret);
}


void PollingModuleInit(void)
{
	static TmrInfo TimerMbs[TIMER_MAX_NUM];
	TmrInitHeader(&gTmrHeader, GetTicks, TimerMbs, GET_ARRAY_NUM(TimerMbs));
}

TmrHeader* GetTmrHd(void)
{
	return &gTmrHeader;
}


void SystemPolling(void)
{
	Polling(GetTmrHd());
}




