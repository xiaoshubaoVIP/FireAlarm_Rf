/*************************************************************************
* Copyright (c) 2018 Shenzhen Xiaojun technology co., Ltd.All right reserved.
* Author: chaofa.yan
* Date: 2018/04/25
* History Information
* Author 		Date 		Description
* chaofa.yan	2018/04/25 	Created the file
*************************************************************************/
#include "Polling.h"
/******************************************************************************
**Description:			TmrInitHeader  init timer list															
** Parameters:          pHeader : timer header struct address    pfGetTicks :timer circle  , pTimer: timer list start address    Mbs :timer num     												
**     Return:   			 pHeader : timer header struct address														
*******************************************************************************/
#if !defined(NULL)
#define NULL ((void*)0)
#endif // MACRO


TmrHeader* TmrInitHeader(TmrHeader* pHeader, pfTimimg pfGetTicks,
							TmrInfo* pTimer, uint32_t Mbs)
{
	if((NULL==pHeader) || (NULL==pfGetTicks) || (NULL==pTimer) || (0==Mbs))
	{
		return (NULL);
	}

	pHeader->pTmrInfo = pTimer;
	pHeader->TmrMbs = Mbs;
	pHeader->pfGetTicks = pfGetTicks;

	while(Mbs > 0)
	{
		pTimer->pfEvent = NULL;
		pTimer++;
		Mbs--;
	}

	return (pHeader);
}

/******************************************************************************
**Description:			TmrInsertTimer															
** Parameters:        pHeader :timer address ;    ReloadCnt : counter num     pfEvent:event function    pCtx :context    												
**     Return:   	timer	handle															
*******************************************************************************/

TmrHdl TmrInsertTimer(TmrHeader* pHeader, uint32_t ReloadCnt, 
							pfTmrCBK pfEvent, void* pCtx)
{
	uint32_t Mbs= 0;
	TmrInfo* pTimer = NULL;
	TmrInfo* pTheTimer = NULL;

	if((NULL==pHeader) || (NULL==pfEvent))
	{
		return (NULL);
	}

	pTimer = pHeader->pTmrInfo;
	Mbs = pHeader->TmrMbs;

	while(Mbs > 0)// has it this event ?
	{
		if(pTimer->pfEvent == pfEvent)// has this event
		{
			pTheTimer = NULL;
			//printf("Timer is have\n");
			break;
		}else  // has other event
		{
			pTimer++;
			Mbs--;
		}
		
	}

	if (0 == Mbs)//has null event
	{
		
		pTimer = pHeader->pTmrInfo;
		Mbs = pHeader->TmrMbs;
		//printf("Timer is none,Mbs=%ld\n",Mbs);
		
		while(Mbs > 0)// has it this event ?
		{
			if(pTimer->pfEvent == NULL)// has null event 
			{
				pTheTimer = pTheTimer==NULL? pTimer: pTheTimer;
				break;
			}else  // has other event
			{
				pTimer++;
				Mbs--;
			}
			
		}
		
	}

	if (Mbs == 0)
	{
		//printf("Timer is Over Err\n");
	}else
	{
		//printf("Timer has %ld,%ld\n",Mbs-1,(uint32_t)pfEvent);
	}

	if(pTheTimer != NULL)
	{
		ReloadCnt = ReloadCnt>0? ReloadCnt: 1; 
		pTheTimer->pCtx = pCtx;
		pTheTimer->CurCnt = ReloadCnt;
		pTheTimer->ReloadCnt = ReloadCnt;
		pTheTimer->pfEvent = pfEvent;
		pfEvent((TmrHdl)pTheTimer, eInsertTimer);
	}

	return ((TmrHdl)pTheTimer);
}

void TmrDeleteTimer(TmrHdl Handle)
{
	TmrInfo* pTimer = (TmrInfo*)Handle;
	pfTmrCBK pfEvent = NULL;

	if(pTimer != NULL)
	{
		pfEvent = pTimer->pfEvent ;
		
		if(pfEvent != NULL)
		{
			pTimer->pfEvent = NULL;
			pfEvent(Handle, eDelTimer);
		}
	}
}

static void CheckTimeOut(TmrInfo* pTimer, uint32_t Ticks)
{
	const pfTmrCBK pfEvent = pTimer->pfEvent;
	uint32_t CurCnt = 0;

	if(pfEvent != NULL)
	{
		CurCnt = pTimer->CurCnt;

		if(CurCnt >= Ticks)
		{
			CurCnt -= Ticks;

			if(CurCnt != 0)
			{
				pTimer->CurCnt = CurCnt;
			}
			else
			{
				pTimer->CurCnt = pTimer->ReloadCnt;
				pfEvent((TmrHdl)pTimer, eTimeOut);
			}
		}
		else
		{
			pTimer->CurCnt = pTimer->ReloadCnt;
			pfEvent((TmrHdl)pTimer, eTimerBlock);
			pfEvent((TmrHdl)pTimer, eTimeOut);
		}
	}
}

void Polling(TmrHeader* pHeader)
{
	uint32_t Mbs = 0;
	TmrInfo* pTimer = NULL;
	uint32_t Ticks = 0;
	
	if(NULL == pHeader)
	{
		return;
	}

	Ticks = pHeader->pfGetTicks();

	if(Ticks > 0)
	{
		pTimer = pHeader->pTmrInfo;
		Mbs = pHeader->TmrMbs;
		
		while(Mbs > 0)
		{
			CheckTimeOut(pTimer, Ticks);
			pTimer++;
			Mbs--;
		}
	}
}

void* TmrGetCtx(TmrHdl Handle)
{
	void* pCtx = NULL;

	if(Handle != NULL)
	{
		pCtx = ((TmrInfo*)Handle)->pCtx;
	}

	return (pCtx);
}

void TmrSetCtx(TmrHdl Handle,void*pCtx)
{
	if(Handle != NULL)
	{
		((TmrInfo*)Handle)->pCtx = pCtx;
	}
}

void TmrSetCurCnt(TmrHdl Handle, uint32_t CurCnt)
{
	TmrInfo* pTimer = (TmrInfo*)Handle;
	
	if(pTimer != NULL)
	{
		pTimer->CurCnt = CurCnt;
	}
}

void TmrResetCurCnt(TmrHdl Handle)
{
	TmrInfo* pTimer = (TmrInfo*)Handle;
	
	if(pTimer != NULL)
	{
		pTimer->CurCnt = pTimer->ReloadCnt;
	}
}

uint32_t TmrGetMbs(const TmrHeader* pHeader)
{
	uint32_t Mbs = 0;

	if(NULL != pHeader)
	{
		Mbs = pHeader->TmrMbs;
	}

	return (Mbs);
}

TmrHdl TmrGetHandleByEvent(const TmrHeader* pHeader, pfTmrCBK pfEvent)
{
	uint32_t Mbs= 0;
	TmrInfo* pTimer = NULL;

	if((NULL==pHeader) || (NULL==pfEvent))
	{
		return (NULL);
	}

	pTimer = pHeader->pTmrInfo;
	Mbs = pHeader->TmrMbs;

	while(Mbs > 0)
	{
		if(pTimer->pfEvent == pfEvent)
		{
			break;
		}

		pTimer++;
		Mbs--;
	}

	
    if(0==Mbs)
    {
		pTimer = NULL;
	}

	return ((TmrHdl)pTimer);
}



