#include "../inc/hal_adc.h"

#define VREF_VALUE 2300 // mv
#define RESOLUTION 12 // 12bit·Ö±æÂÊ


/**********************************************************************/
//Description:	hal_adc_init()									
//Parameters:                        									
//Return:   															
//Date:           
//Author:		quanwu.xu  
/**********************************************************************/
void hal_adc_init(void)
{

}

/**********************************************************************/
//Description:	hal_adc_read()									
//Parameters:                        									
//Return:   															
//Date:           
//Author:		quanwu.xu  
/**********************************************************************/
unsigned short hal_adc_read(adc_id_t id)
{
	return 0;
}

/**********************************************************************/
//Description:	hal_adc_read_vol()									
//Parameters:                        									
//Return:   															
//Date:           
//Author:		quanwu.xu  
/**********************************************************************/
unsigned short hal_adc_read_vol(adc_id_t id)
{
    uint32_t value;

    value = hal_adc_read(id);

    return (value * VREF_VALUE) >> RESOLUTION;
}

