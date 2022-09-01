/*****************************************************************************
* Product:  QF/C
* Last Updated for Version: 3.1.04
* Date of the Last Update:  Dec 01, 2005
*
* Copyright (C) 2002-2005 Quantum Leaps, LLC. All rights reserved.
*
* This software may be distributed and modified under the terms of the GNU
* General Public License version 2 (GPL) as published by the Free Software
* Foundation and appearing in the file GPL.TXT included in the packaging of
* this file. Please note that GPL Section 2[b] requires that all works based
* on this software must also be made publicly available under the terms of
* the GPL ("Copyleft").
*
* Alternatively, this software may be distributed and modified under the
* terms of Quantum Leaps commercial licenses, which expressly allow the
* licensees to retain the proprietary status of their code. The licensees
* who use this software under one of Quantum Leaps commercial licenses do
* not use this software under the GPL and therefore are not subject to any
* of its terms.
*
* Contact information:
* Quantum Leaps Web site:  http://www.quantum-leaps.com
* Quantum Leaps licensing: http://www.quantum-leaps.com/licensing
* Quantum Leaps products:  http://www.quantum-leaps.com/products
* e-mail:                  sales@quantum-leaps.com
*****************************************************************************/
#include "qep/inc/qep_pkg.h"
#include "qep/inc/qassert.h"
/*..........................................................................*/
/** \ingroup qf
* \file qeq_fifo.c
* \brief QEQueue_postFIFO() definition. NOTE: this function is used for
* the "raw" thread-safe queues and NOT for the queues of active objects.
*/

//Q_DEFINE_THIS_MODULE(qeq_fifo)
/*..........................................................................*/
void QEQueue_init(QEQueue *me, QEvent const *qSto[], QEQueueCtr qLen) {
    me->ring__     = &qSto[0];
    me->nTot__     = qLen;
    me->end__      = me->nTot__;
    me->head__     = (QEQueueCtr)0;
    me->tail__     = (QEQueueCtr)0;
    me->nUsed__    = (QEQueueCtr)0;
    me->nMax__     = (QEQueueCtr)0;
    me->frontEvt__ = (QEvent const *)0;           /* no events in the queue */
}


/*..........................................................................*/
QEvent const *QEQueue_get(QEQueue *me) {
	QEvent const *e;

	if (me->frontEvt__ == (QEvent const *)0) {		 /* is the queue empty? */
		e = (QEvent const *)0;			 /* no event available at this time */
	}
	else {										  /* the queue is not empty */
		e = me->frontEvt__;
		if (me->nUsed__ != (QEQueueCtr)0) {/*any events in the ring buffer? */
			me->frontEvt__ = me->ring__[me->tail__];/* remove from the tail */
			++me->tail__;
			if (me->tail__ == me->end__) {			/* need to wrap around? */
				me->tail__ = (QEQueueCtr)0; 			   /* wrap the tail */
			}

			--me->nUsed__;			   /* one less event in the ring buffer */

			 
		}
		else {
			me->frontEvt__ = (QEvent const *)0;  /* the queue becomes empty */
			
		}
	}
    
	return e;
}


/*..........................................................................*/
void QEQueue_postFIFO(QEQueue *me, QEvent const *e) {
 
    // if (e->attrQF__ != (uint8_t)0) {                 /* is it a pool event? */
    //     ++((QEvent *)e)->attrQF__;       /* increment the reference counter */
    //                /* NOTE: cast the 'const' away, which is legitimate because
    //                 * it's a pool event
    //                 */
    // }

    if (me->frontEvt__ == (QEvent const *)0) {       /* is the queue empty? */
        me->frontEvt__ = e;                       /* deliver event directly */
    }
    else {            /* queue is not empty, leave event in the ring-buffer */
            /* the queue must be able to accept the event (cannot overflow) */
        Q_ASSERT(me->nUsed__ < me->nTot__);
        me->ring__[me->head__] = e;  /* insert event into the buffer (FIFO) */
        ++me->head__;
        if (me->head__ == me->end__) {
            me->head__ = (QEQueueCtr)0;                    /* wrap the head */
        }

        ++me->nUsed__;                           /* update number of events */
        // if (me->nUsed__ > me->nMax__) {
        //     me->nMax__ = me->nUsed__;          /* store maximum used so far */
        // }
    }
}

/*..........................................................................*/
void QEQueue_postLIFO(QEQueue *me, QEvent const *e) {

    // if (e->attrQF__ != (uint8_t)0) {                 /* is it a pool event? */
    //     ++((QEvent *)e)->attrQF__;       /* increment the reference counter */
    //                /* NOTE: cast the 'const' away, which is legitimate because
    //                 * it's a pool event
    //                 */
    // }

    if (me->frontEvt__ != (QEvent const *)0) {   /* is the queue not empty? */
            /* the queue must be able to accept the event (cannot overflow) */
        Q_ASSERT(me->nUsed__ < me->nTot__);
        if (me->tail__ == (QEQueueCtr)0) {        /* need to wrap the tail? */
            me->tail__ = (QEQueueCtr)(me->end__ - 1);
        }
        else {
            --me->tail__;
        }
        me->ring__[me->tail__] = me->frontEvt__;/* buffer the old front evt */

        ++me->nUsed__;                           /* update number of events */
        // if (me->nUsed__ > me->nMax__) {
        //     me->nMax__ = me->nUsed__;          /* store maximum used so far */
        // }
    }

    me->frontEvt__ = e;                 /* stick the new event to the front */

}

