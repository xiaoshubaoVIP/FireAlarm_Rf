#include "qep_port.h"

#define MainTaskQueue_Length            (10)

//static QFsm  super_;  //QP system
static QEvent const *SystemQueueSto[MainTaskQueue_Length];
static QEQueue eQueue__;

void QEP_Init(QFsm * me, QState init)
{
	QEvent ie;

	QFsm_ctor_(me, init);
	QEQueue_init(&eQueue__, SystemQueueSto, Q_DIM(SystemQueueSto)); //QUEUE FOR EVENT
	QFsm_init(me, (QEvent const *)&ie);
}

void QEP_Handler(QFsm * me)
{
	QEvent *e;

	while ((e = (QEvent *)QEQueue_get(&eQueue__)) != 0)
	{
		QFsm_dispatch(me, e);
	}
}


void QEP_Post(QEvent const *pEvt)
{
	static QEvent des[MainTaskQueue_Length];
	static uint8_t des_count = 0;

	des[des_count].sig = pEvt->sig;
	des[des_count].Value1 = pEvt->Value1;
	des[des_count].Value2 = pEvt->Value2;
	des[des_count].Value3 = pEvt->Value3;
	des[des_count].Value4 = pEvt->Value4;

	QEQueue_postFIFO(&eQueue__, &des[des_count]);

	des_count++;
	if (des_count > MainTaskQueue_Length)
	{
		des_count = 0;
	}
}

