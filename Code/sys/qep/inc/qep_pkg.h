/****************************************************************************/
/*	File   :             F:\Smoker\WiFiSmoke\trunk\src\include\qep\qep_pkg.h               */
/*	Author :                       songjiang                                */
/*	Version:                          V1.0                                  */
/*	Date   :                    2019/5/16,16:11:40                         */
/********************************Modify History******************************/
/*   yy/mm/dd     F. Lastname    Description of Modification                */
/****************************************************************************/
/*	Attention:                                                              */
/*	CopyRight:                CopyRight 2017--2025                          */
/*           Shenzhen Xiaojun Technology Co., Ltd.  Allright Reserved        */
/*	Project :                      SC01                           */
/****************************************************************************/

#ifndef qep_pkg_h
#define qep_pkg_h

/** \ingroup qep
* \file qep_pkg.h
* \brief Internal (package scope) QEP/C interface.
*/

#include "qep_port.h"                                           /* QEP port */
#include <stdint.h>

#ifdef Q_SPY
    #include "qs_port.h"                                         /* QS port */
#else
    #define QS_BEGIN_(rec_, refObj_, obj_)  if (0) { ((void)0)
    #define QS_END_()                       } else ((void)0)
    #define QS_U8_(data_)                   ((void)0)
    #define QS_U16_(data_)                  ((void)0)
    #define QS_U32_(data_)                  ((void)0)
    #define QS_TIME_()                      ((void)0)
    #define QS_SIG_(sig_)                   ((void)0)
    #define QS_OBJ_(obj_)                   ((void)0)
    #define QS_FUN_(fun_)                   ((void)0)
#endif

extern QEvent const QEP_reservedEvt_[];   /**< preallocated reserved events */
#define QF_CRIT_ENTRY_()	disableInterrupts()
#define QF_CRIT_EXIT_()	    enableInterrupts()

/** internal QEP constants */
enum QEPConst {
    QEP_EMPTY_SIG_ = 0,             /**< empty signal for internal use only */

    /** maximum depth of state nesting (including the top level),
     * must be >= 3
     */
    QEP_MAX_NEST_DEPTH_ = 6
};

#endif                                                         /* qep_pkg_h */
