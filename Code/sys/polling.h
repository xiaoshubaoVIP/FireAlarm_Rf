/*************************************************************************
* Copyright (c) 2018 Shenzhen Xiaojun technology co. Ltd.All right reserved.
* Author: chaofa.yan
* Date: 2018/04/25
* History Information
* Author 		Date 		Description
* chaofa.yan	2018/04/25 	Created the file
*************************************************************************/

#ifndef __POLLING_H__
#define __POLLING_H__


#define TICK_UNIT 1 //1ms
//#define TICK_UNIT 10 //10ms
// #define TICK_UNIT (5) //5ms

#include <stdint.h>

//unit of Period is ms
#define TMR_PERIOD(Period) \
	((Period)<TICK_UNIT? 1: (((Period)+TICK_UNIT/2)/TICK_UNIT))

#define TICK_IDLE_UNIT (SLEEP_TIME) //10s

//unit of Period is ms
#define TMR_IDLE_PERIOD(Period) \
	((Period)<TICK_IDLE_UNIT? 1: (((Period)+TICK_IDLE_UNIT/2)/TICK_IDLE_UNIT))

typedef void* TmrHdl;

typedef enum _eTimerCmd
{
	eTimeOut = 0,
	eInsertTimer,
	eDelTimer,
	eTimerBlock,//
	eTmrCmdMax,
}eTmrCmd;

typedef void (*pfTmrCBK)(TmrHdl Handle, eTmrCmd eCmd);

typedef struct _TimerInfo
{
	pfTmrCBK 	pfEvent;
	void* 		pCtx;
	uint32_t 		CurCnt;
	uint32_t 		ReloadCnt;
}TmrInfo;

typedef uint32_t (*pfTimimg)(void);

typedef struct _TimerInfoHeader
{
	TmrInfo* 	pTmrInfo;
	uint32_t 	TmrMbs;
	pfTimimg 	pfGetTicks;
}TmrHeader;

extern TmrHeader* TmrInitHeader(TmrHeader* pHeader, pfTimimg pfGetTicks, TmrInfo* pTimer, uint32_t Mbs);
extern TmrHdl TmrInsertTimer(TmrHeader* pHeader, uint32_t ReloadCnt, pfTmrCBK pfEvent, void* pCtx);
extern void TmrDeleteTimer(TmrHdl Handle);
extern void Polling(TmrHeader* pHeader);

extern void* TmrGetCtx(TmrHdl Handle);
extern void TmrSetCtx(TmrHdl Handle,void*pCtx);

extern void TmrSetCurCnt(TmrHdl Handle, uint32_t CurCnt);
extern void TmrResetCurCnt(TmrHdl Handle);
extern uint32_t TmrGetMbs(const TmrHeader* pHeader);
extern TmrHdl TmrGetHandleByEvent(const TmrHeader* pHeader, pfTmrCBK pfEvent);

#endif


