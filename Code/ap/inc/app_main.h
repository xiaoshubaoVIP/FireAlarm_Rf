#ifndef  __APP_MAIN_H__
#define  __APP_MAIN_H__


#ifdef __cplusplus
extern "C"
{
#endif

#include "common.h"
#include "base_types.h"
#include "qep_port.h"
#include "qassert.h"



void PrintfVersion(void);
void EntryLowpwrMode(void);
void SysIdleModeHandleFunc(SystemApiTag  *me,QEvent *e);
void ApplicationInit(void);

#ifdef __cplusplus
}
#endif

#endif


