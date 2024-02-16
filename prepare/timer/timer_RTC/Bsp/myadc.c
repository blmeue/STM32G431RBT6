#include "myadc.h"


double get_ADC(ADC_HandleTypeDef *pin)
{
    //读出来的值
    unsigned int adc;
    
    //开启ADC
    HAL_ADC_Start(pin);
    
    //读取ADC的值
    adc=HAL_ADC_GetValue(pin);
    
    return adc*(3.3/4096);
}
