/*************************************************************************
* Copyright (c) 2018 Shenzhen Xiaojun technology co., Ltd.All right reserved.
* Author: chaofa.yan
* Date: 2018/07/04
* History Information
* Author 		Date 		Description
* chaofa.yan	2018/07/04 	Created the file
*************************************************************************/

#ifndef __SCENE_INSTANCE_H__
#define __SCENE_INSTANCE_H__

#include "polling.h"

//polling
#define TimeOut10ms     ((uint32_t)10)
#define TimeOut50ms     ((uint32_t)50)
#define TimeOut200ms	((uint32_t)200)
#define TimeOut500ms	((uint32_t)500)
#define TimeOut1s       ((uint32_t)1000)
#define TimeOut2_5s     ((uint32_t)2500)
#define TimeOut3s       ((uint32_t)3000)
#define TimeOut4s       ((uint32_t)4000)
#define TimeOut5s       ((uint32_t)5000)
#define TimeOut10s      ((uint32_t)10000)
#define TimeOut30s      ((uint32_t)30000)
#define TimeOut60s      ((uint32_t)60000)
#define TimeOut180s     ((uint32_t)180000)
#define TimeOut300s     ((uint32_t)300000)
#define Timeout1200s    ((uint32_t)1200000)			//20min
#define Timeout1hour    ((uint32_t)3600000)			//60min
#define SYS_TIME_MAX    (0x0F)

#if 0
//#define TICK_UNIT 10 //10ms
#define TICK_UNIT 5 //ms

//unit of Period is ms
#define TMR_PERIOD(Period) \
	((Period)<TICK_UNIT? 1: (((Period)+TICK_UNIT/2)/TICK_UNIT)) 
#endif

#define TIMER_MAX_NUM    (16)

typedef enum{
	TimeHeader = 0,
	TimeHeaderIdle,
}eTimeType;


extern TmrHeader* GetTmrHd(void);
extern TmrHeader* GetTmrIdleHd(void);

extern void PollingModuleInit(void);
extern void SystemPolling(void);
extern void SystemPollingIdle(void);

#endif

