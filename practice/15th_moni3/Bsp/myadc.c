#include "myadc.h"

double Get_Adc(ADC_HandleTypeDef *pin)
{
    int ret;
    
    //����ADC
    HAL_ADC_Start(pin);
    
    //��ȡADC��ֵ
    ret=HAL_ADC_GetValue(pin);
    
    return ret*(3.3/4096);
}
