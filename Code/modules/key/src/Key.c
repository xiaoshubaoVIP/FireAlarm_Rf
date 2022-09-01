#include "key.h"
#include "bsp_config.h"

#define	KEY_1_is_L() 	(HalGpioRead(PIN_KEY) == PIN_RESET)

#define KEYCOUNTERNUMLONG 				    (300)		    //3000MS
#define KEYCOUNTERNUMCONTINUOUS_LIMIT 	    (100)			//300MS
#define KEYCOUNTERNUMINTERVAL_LIMIT 		(80)		    //200MS
#define KEYCOUNTERNUMSHORT_LIMIT 		    (70)			//400MS
#define KEYCOUNTERNUMSHORT 				    (2)			    //20MS
#define KEYCOUNTERREPEAT 					(20)		    //20MS

KeyApi KeyApiTag;
KeyApi* pKeyApiTag = &KeyApiTag;

static TmrHdl DoubleKeyScanTimeoutHeader = NULL;
static void PollingDoubleClickScan(TmrHdl Handle, eTmrCmd eCmd);

/**********************************************************************/
//Description:	Key_Init()													
//Parameters:                        										
//Return:   																
//Date:           
//Author:	quanwu.xu  
/**********************************************************************/
void KeyInit(void)
{ 
	pKeyApiTag->keyCounter  = 0;	
	pKeyApiTag->keyClickFlag = 0;
	pKeyApiTag->keyClickCnt = 0;
	pKeyApiTag->keyValue = KEY_DEFAULT;
	pKeyApiTag->keyCurrent =  KEY_DEFAULT;	
	pKeyApiTag->keyValueLast = KEY_DEFAULT;
	pKeyApiTag->keyType = TYPE_NONE;

	HalGpioSetMode(PIN_KEY, PIN_MODE_INPUT_PULLUP);
	HalGpioSetIRQMode(PIN_KEY, PIN_IRQ_FALLING);

	TmrInsertTimer(GetTmrHd(),TMR_PERIOD(TimeOut10ms), PollingKeyScan, NULL);
}

/**********************************************************************/
//Description:	GetKeyApiTag()															  
//Parameters:                        												  
//Return:   																  
//Date:           
//Author:	quanwu.xu  
/**********************************************************************/
KeyApi* GetKeyApiTag(void)
{
	return pKeyApiTag;
}

/**********************************************************************/
//Description:	Key_Init()															  
//Parameters:                        												  
//Return:   																  
//Date:           
//Author:	quanwu.xu  
/**********************************************************************/
uint8_t GetKeyClickFlag(void)
{
	return pKeyApiTag->keyClickFlag;
}


/**********************************************************************/
//Description:	Key_Init()															  
//Parameters:                        												  
//Return:   																  
//Date:           
//Author:	quanwu.xu  
/**********************************************************************/
void SetKeyClickFlag(uint8_t Flag)
{
	pKeyApiTag->keyClickFlag = Flag;
}



/**********************************************************************/
//Description:	Key_Init()															  
//Parameters:                        												  
//Return:   																  
//Date:           
//Author:	quanwu.xu  
/**********************************************************************/
static uint8_t KeyGpioValue()
{	
	uint8_t ret = KEY_DEFAULT;	 

	if(KEY_1_is_L())
	{
		ret |= KEY1_PRESS;
	}	
	return ret;
}


/**********************************************************************/
//Description:	Key_Init()															  
//Parameters:                        												  
//Return:   																  
//Date:           
//Author:	quanwu.xu  
/**********************************************************************/
void KeyCheck(void)
{
	static uint8_t flag = 0 ;
	KeyApi* pKeyApi = GetKeyApiTag();
	if (GetKeyClickFlag())
	{
		flag = 0;
	}
	else
	{
		if (KeyGpioValue() == KEY_DEFAULT)			//  high
		{
			ResetSysSleepFlag(eSysActiveFlag_KeyPress);
			flag = 1;
			SetKeyClickFlag(0);
			pKeyApi->keyCounter = 0;	
			pKeyApi->keyCurrent = KEY_DEFAULT;
			
		}
		else
		{
			SetSysSleepFlag(eSysActiveFlag_KeyPress);
			if(flag == 1)
			{
				SetKeyClickFlag(1);
			}
		}
	}
}


/**********************************************************************/
//Description:	Key_Init()															  
//Parameters:                        												  
//Return:   																  
//Date:           quanwu.xu  
/**********************************************************************/
void KeyGpioScan(void)
{
	KeyApi* pKeyApi = GetKeyApiTag();
	uint8_t KeyTemp = KeyGpioValue();

	if(GetKeyClickFlag())
	{
		if (pKeyApi->keyCurrent == KeyTemp)						//按下按键												
		{
			pKeyApi->keyCounter++;
			if(pKeyApi->keyCounter >= KEYCOUNTERNUMLONG)		//10 ms * 300 = 3000ms
			{	
				pKeyApi->keyType = TYPE_LONG_CLICK;
				pKeyApi->keyValue = ( KEYValue)(pKeyApi->keyCurrent & 0x7F);
				SetKeyClickFlag(0);
			}
		}
		else																								
		{
			if(KeyTemp == KEY_DEFAULT)
			{
				if((pKeyApi->keyCounter >= KEYCOUNTERNUMSHORT)	&& (pKeyApi->keyCounter 
												< KEYCOUNTERNUMSHORT_LIMIT))//短按按键
				{
					pKeyApi->keyType = TYPE_WAIT;
					pKeyApi->keyValue = (KEYValue)(pKeyApi->keyCurrent & 0x7F);
					SetKeyClickFlag(0);

					pKeyApi->keyClickCnt++;
					TmrDeleteTimer(DoubleKeyScanTimeoutHeader);
					DoubleKeyScanTimeoutHeader = TmrInsertTimer(GetTmrHd(),
										TMR_PERIOD(TimeOut1s), PollingDoubleClickScan, NULL);;
				}
				else
				{
					pKeyApi->keyCounter = 0;
					pKeyApi->keyValue = KEY_DEFAULT;
					pKeyApi->keyCurrent = KEY_DEFAULT;
					SetKeyClickFlag(0);
				}		
			}
			else
			{
				pKeyApi->keyCounter  = 0;
				pKeyApi->keyValue = KEY_DEFAULT; 
				pKeyApi->keyCurrent = KEY_DEFAULT;
			}
		}
		pKeyApi->keyCurrent =  (KEYValue)KeyTemp;
	}	
}


/**********************************************************************/
//Description:	isNewKeyMsg()															  
//Parameters:                        												  
//Return:   																  
//Date:           
//Authot:  	quanwu.xu  
/**********************************************************************/
bool isNewKeyMsg(void)
{
	bool ret = false;
	KeyApi* pKeyApi = GetKeyApiTag();
	if((pKeyApi->keyValue != KEY_DEFAULT) && (pKeyApi->keyType != TYPE_WAIT))
	{
		pKeyApi->keyValueLast = pKeyApi->keyValue;
		pKeyApi->keyValue = KEY_DEFAULT;
		ret = true;
	}
	return ret; 
}


/**********************************************************************/
//Description:	Key_Init()															  
//Parameters:                        												  
//Return:   																  
//Date:           
//Author:	quanwu.xu  
/**********************************************************************/
uint8_t GetKeyType(void)
{	
	return pKeyApiTag->keyType;
}


/**********************************************************************/
//Description:	PollingDoubleClickScan()															  
//Parameters:                        												  
//Return:   																  
//Date:         quanwu.xu  
/**********************************************************************/
static void PollingDoubleClickScan(TmrHdl Handle, eTmrCmd eCmd)
{
	KeyApi* pKeyApi = GetKeyApiTag();
	
	Handle = Handle;
	switch(eCmd)
	{
		case eInsertTimer:
			SetSysSleepFlag(eSysActiveFlag_KeyWait);
			break;
			
		case eTimeOut:
			pKeyApi->keyType = (KEYType)(pKeyApi->keyClickCnt + 2);
			break;

		case eDelTimer:
			ResetSysSleepFlag(eSysActiveFlag_KeyWait);
			break;
			
		default:
			break;
	}
}


/**********************************************************************/
//Description:	PollingKeyScan()															  
//Parameters:                        												  
//Return:   																  
//Date:           quanwu.xu  
/**********************************************************************/
void PollingKeyScan(TmrHdl Handle, eTmrCmd eCmd)
{
	Handle = Handle;
	switch(eCmd)
	{
		case eInsertTimer:
			break;

		case eTimeOut:
		{
			if(GetKeyClickFlag())			
			{
				KeyGpioScan();
			}
		}
		break;

		default:
			break;
	}
}


/**********************************************************************/
//Description:	Key1Handle()															  
//Parameters:                        												  
//Return:   																  
//Date:           
//Author:	quanwu.xu  
/**********************************************************************/
void Key1Handle(KEYType KeyType)
{
	QEvent KeyEvt = {0};
	switch(KeyType)
	{
		case TYPE_SHORT_CLICK:
			KeyEvt.sig = Q_eKeyEventKey1Clik;
			break;
	
		case TYPE_DOUBLE_CLICK:
			break;
	
		case TYPE_LONG_CLICK:
			KeyEvt.sig = Q_eKeyEventKey1LongClik;
			break;
	
		default:
			break;
	}	
	QEP_Post(&KeyEvt);
}

/**********************************************************************/
//Description:	Key_Handle()															  
//Parameters:																		  
//Return:																	  
//Date: 		quanwu.xu  
/**********************************************************************/
void KeyHandleMsg(void)
{
	KeyApi *pKeyApi = GetKeyApiTag();
	
	KeyCheck();
	if(isNewKeyMsg() == true)
	{
		switch (pKeyApi->keyValueLast)
		{
			case KEY1_PRESS:
			{
				debug_printf("key 1, %d\n", pKeyApi->keyType);
				Key1Handle(pKeyApi->keyType);
			}
			break;

			default:
				break;
		}

		pKeyApi->keyClickCnt = 0;
		pKeyApi->keyType = TYPE_NONE;
		pKeyApi->keyValueLast = KEY_DEFAULT;
		TmrDeleteTimer(DoubleKeyScanTimeoutHeader);
	}
}






