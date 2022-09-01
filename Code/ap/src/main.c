#include "bsp_config.h"
#include "modules_config.h"

#include "app_main.h"
#include "app_radio.h"
#include "app_uart.h"


/**********************************************************************/
//Description:	main()													
//Parameters:                        									
//Return:   															
//Date:           
//Author:		quanwu.xu  
/**********************************************************************/
int32_t main(void)
{
	BoardInit();
	ApplicationInit();
	
	while(1)
	{
		SystemPolling();
		KeyHandleMsg();
		UartService();
		RadioService();
		QEP_Handler((QFsm *)&SystemApiTagObj);
		EntryLowpwrMode();
	}
}

