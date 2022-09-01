/****************************************************************************/
/*	File   :             F:\Smoker\WiFiSmoke\trunk\src\qep\qfsm_dis.c               */
/*	Author :                       songjiang                                */
/*	Version:                          V1.0                                  */
/*	Date   :                    2019/5/16,16:12:0                         */
/********************************Modify History******************************/
/*   yy/mm/dd     F. Lastname    Description of Modification                */
/****************************************************************************/
/*	Attention:                                                              */
/*	CopyRight:                CopyRight 2017--2025                          */
/*           Shenzhen Xiaojun Technology Co., Ltd.  Allright Reserved        */
/*	Project :                      SC01                           */
/****************************************************************************/

#include "qep/inc/qep_pkg.h"
#include "qep/inc/qassert.h"

/** \ingroup qep
* \file qfsm_dis.c
* \brief QFsm_dispatch() implementation.
*/

void QFsm_init(QFsm *me, QEvent const *e) {
    QState initial;
    Q_REQUIRE(me->state__.fsm != (QState)0);         /* must be initialized */
    initial = me->state__.fsm;              /* save the initial pseudostate */
    (*initial)(me, e);                    /* trigger the initial transition */
    Q_ASSERT(initial != me->state__.fsm);     /* cannot stay in the initial */
    (*me->state__.fsm)(me, &QEP_reservedEvt_[Q_ENTRY_SIG]); /* enter target */

}

void QFsm_dispatch(QFsm *me, QEvent const *e) {
    QState s = me->state__.fsm;
    (*s)(me, e);          /* process the event in the current state handler */
    if (me->tran__ != Q_TRAN_NONE_TYPE) {
        (*s)(me, &QEP_reservedEvt_[Q_EXIT_SIG]);         /* exit the source */
                                                        /* enter the target */
        (*me->state__.fsm)(me, &QEP_reservedEvt_[Q_ENTRY_SIG]);

        me->tran__ = Q_TRAN_NONE_TYPE;     /* get ready for next transition */

        QS_BEGIN_(QS_QEP_TRAN, QS_smObj_, me);
            QS_TIME_();                                       /* time stamp */
            QS_SIG_(e->sig);                     /* the signal of the event */
            QS_OBJ_(me);                       /* this state machine object */
            QS_FUN_(s);                     /* the source of the transition */
            QS_FUN_(me->state__.fsm);               /* the new active state */
        QS_END_();

    }
    else {
        QS_BEGIN_(QS_QEP_INTERN_TRAN, QS_smObj_, me);
            QS_TIME_();                                       /* time stamp */
            QS_SIG_(e->sig);                     /* the signal of the event */
            QS_OBJ_(me);                       /* this state machine object */
            QS_FUN_(s);                 /* the state that handled the event */
        QS_END_();
    }
}

