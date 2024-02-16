#include "myadc.h"


double get_ADC(ADC_HandleTypeDef *pin)
{
    //��������ֵ
    unsigned int adc;
    
    //����ADC
    HAL_ADC_Start(pin);
    
    //��ȡADC��ֵ
    adc=HAL_ADC_GetValue(pin);
    
    return adc*(3.3/4096);
}
