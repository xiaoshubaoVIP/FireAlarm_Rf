#ifndef __HAL_ADC_H__
#define __HAL_ADC_H__

#define ADCNUM (2)

/* �ر���־��ӡ��ֻ��ģ�� */
#define UNUSED_LOG

/**
 * @brief adc�豸��
 */
typedef enum
{
    ADC_CO,         /* CO������ */
    ADC_SMOKE,      /* SMOKE������ */
    ADC_TEMP,       /* �¶ȴ����� */
    ADC_BATTERY,    /* ��� */
} adc_id_t;

void hal_adc_init(void);
unsigned short hal_adc_read(adc_id_t id);
unsigned short hal_adc_read_vol(adc_id_t id);

// void hal_adc_deinit(void);
// void hal_adc_mode();
// void hal_adc_enable(adc_id_t id);
// void hal_adc_disable(adc_id_t id);

#endif
