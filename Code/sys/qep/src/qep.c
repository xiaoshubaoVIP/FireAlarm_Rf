/****************************************************************************/
/*	File   :             F:\Smoker\WiFiSmoke\trunk\src\qep\qep.c               */
/*	Author :                       songjiang                                */
/*	Version:                          V1.0                                  */
/*	Date   :                    2019/5/16,16:11:18                         */
/********************************Modify History******************************/
/*   yy/mm/dd     F. Lastname    Description of Modification                */
/****************************************************************************/
/*	Attention:                                                              */
/*	CopyRight:                CopyRight 2017--2025                          */
/*           Shenzhen Xiaojun Technology Co., Ltd.  Allright Reserved        */
/*	Project :                      SC01                           */
/****************************************************************************/
#include "qep/inc/qep_pkg.h"

/** \ingroup qep
* \file qep.c
* \brief ::QEP_reservedEvt_ definition and QEP_getVersion() implementation.
*/
/* Package-scope objects ---------------------------------------------------*/
QEvent const QEP_reservedEvt_[] = {
    { (QSignal)QEP_EMPTY_SIG_, (uint8_t)0 },
    { (QSignal)Q_ENTRY_SIG,    (uint8_t)0 },
    { (QSignal)Q_EXIT_SIG,     (uint8_t)0 },
    { (QSignal)Q_INIT_SIG,     (uint8_t)0 }
};

#if 0
/*..........................................................................*/
/*lint -e970 -e971 */      /* ignore MISRA rules 13 and 14 in this function */
char const Q_ROM *QEP_getVersion(void) {
    static char const Q_ROM version[] = "3.2.01";
    return version;
}

#endif

