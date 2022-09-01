#ifndef __DEBUG_H__
#define __DEBUG_H__

/* 日志打印开关 */
// #define DISABLE_LOG 

/* 
    日志等级
    0:ERROR
    1:WARN
    2:INFO
 */
#define LOG_LEVLE INFO_LOG_LEVEL

#define ERR_LOG_LEVEL 0
#define WARN_LOG_LEVEL 1
#define INFO_LOG_LEVEL 2

#if defined (UNUSED_LOG) || defined (DISABLE_LOG)
    #define debug_printbuff(buff, len, fmt, ...)
    #define debug_info(fmt, arg...)
    #define debug_warning(fmt, arg...)
    #define debug_error(fmt, arg...)
#else
    #include <stdio.h>
    //#define debug_info(fmt, arg...) printf("[I]%s(%d):" fmt "\n", remove_path(__FILE__), __LINE__, ##arg)
    //#define debug_warning(fmt, arg...) printf("[W]%s(%d):" fmt "\n", remove_path(__FILE__), __LINE__, ##arg)
    //#define debug_error(fmt, arg...) printf("[E]%s(%d):" fmt "\n", remove_path(__FILE__), __LINE__, ##arg)
    //#define debug_printbuff(buff, len, fmt, arg...) \
    //do{ \
    //    unsigned short i; \
    //    printf("%s(%d):" fmt, remove_path(__FILE__), __LINE__, ##arg); \
    //    for (i = 0; i < len; i++)\
    //        printf("%x ", buff[i]); \
    //    printf("\n"); \
    //}while(0)

    //char* remove_path(char* path);

	#define debug_printf(fmt,args...) printf(fmt, ##args)
	#define debug_info(fmt, arg...) printf("[I]%s(%d):" fmt "\n", __FUNCTION__, __LINE__, ##arg)
	#define debug_warning(fmt, arg...) printf("[W]%s(%d):" fmt "\n",__FUNCTION__, __LINE__, ##arg)
	#define debug_error(fmt, arg...) printf("[E]%s(%d):" fmt "\n", __FUNCTION__, __LINE__, ##arg)
#endif

#undef UNUSED_LOG
#endif
