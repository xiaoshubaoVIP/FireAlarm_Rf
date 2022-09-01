#ifndef __HAL_ADC_H__
#define __HAL_ADC_H__

#include <stdint.h>

#define ADCNUM (2)

/* 关闭日志打印，只本模块 */
#define UNUSED_LOG

/**
 * @brief adc设备号
 */
typedef enum
{
    ADC_CO,         /* CO传感器 */
    ADC_SMOKE,      /* SMOKE传感器 */
    ADC_TEMP,       /* 温度传感器 */
    ADC_BATTERY,    /* 电池 */
} adc_id_t;

void hal_adc_init(void);
unsigned short hal_adc_read(adc_id_t id);
unsigned short hal_adc_read_vol(adc_id_t id);

// void hal_adc_deinit(void);
// void hal_adc_mode();
// void hal_adc_enable(adc_id_t id);
// void hal_adc_disable(adc_id_t id);

#endif
