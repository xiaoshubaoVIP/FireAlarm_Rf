#ifndef __BSP_CONFIG_H__
#define __BSP_CONFIG_H__

#include "config.h"

#ifdef SUPPORT_HC32L100

#include "HC32L110/inc/hal_flash.h"
#include "HC32L110/inc/hal_adc.h"
#include "HC32L110/inc/hal_rtc.h"
#include "HC32L110/inc/hal_uart.h"
#include "HC32L110/inc/hal_tim.h"
#include "HC32L110/inc/hal_gpio.h"
#include "HC32L110/inc/hal_watchdog.h"
#include "HC32L110/inc/hal_tick.h"
#include "HC32L110/inc/board.h"

#elif defined(SUPPORT_BAT32G133)

#include "Bat32G133/inc/hal_flash.h"
#include "Bat32G133/inc/hal_adc.h"
#include "Bat32G133/inc/hal_rtc.h"
#include "Bat32G133/inc/hal_uart.h"
#include "Bat32G133/inc/hal_tim.h"
#include "Bat32G133/inc/hal_gpio.h"
#include "Bat32G133/inc/hal_watchdog.h"
#include "Bat32G133/inc/hal_tick.h"
#include "Bat32G133/inc/board.h"

#endif

#endif
