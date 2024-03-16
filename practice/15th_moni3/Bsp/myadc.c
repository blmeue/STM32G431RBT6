#include "myadc.h"

double Get_Adc(ADC_HandleTypeDef *pin)
{
    int ret;
    
    //开启ADC
    HAL_ADC_Start(pin);
    
    //获取ADC的值
    ret=HAL_ADC_GetValue(pin);
    
    return ret*(3.3/4096);
}
