#ifndef __QEP_PORT_H__
#define __QEP_PORT_H__

 /* no special extended keywords necessary to allocate const objects to ROM */
#define Q_ROM

#define QEP_SIGNAL_SIZE 1


#include "./qep/inc/qep.h"               /* QEP platform-independent public interface */
#include "./qep/inc/qequeue.h"
#include "./qep/inc/qassert.h"

void QEP_Handler(QFsm * me);
void QEP_Post(QEvent const *pEvt);
void QEP_Init(QFsm * me, QState init);

#endif                                                        /* qep_port_h */
