#ifndef  __COMMON_H__
#define  __COMMON_H__


#ifdef __cplusplus
extern "C"
{
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"
#include "system.h"
#include "debug.h"

#include "event.h"
#include "polling.h"
#include "SceneInstance.h"
#include "qep_port.h"


#define GET_ARRAY_NUM(obj) (sizeof(obj) / sizeof(obj[0]))
#define assert_param(expr) ((void)0)

#ifdef __cplusplus
}
#endif

#endif


