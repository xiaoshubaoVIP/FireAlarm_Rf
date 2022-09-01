#include "common.h"
#include "bsp_config.h"
#include "modules_config.h"

#include "app_radio.h"
#include "app_rtc.h"

#define HEARBEAT_CYC_CNT	((uint8_t)10)

void ReadRtcTime(void)
{
	QEvent RtcIntCallBack = {0};
	static uint8_t HearbeatTimeCnt = 0;
	
	if((GetRfPairedMode() == ePairedHub) && (++HearbeatTimeCnt > HEARBEAT_CYC_CNT))
	{
		HearbeatTimeCnt = 0;
		RtcIntCallBack.sig = Q_eSystemEventUpdateHeartbeat;
		RtcIntCallBack.Value1 = HearbeatMsgUpdate;
		QEP_Post(&RtcIntCallBack);
	}
}

