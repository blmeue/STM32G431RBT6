#include "myadc.h"

double get_adc(ADC_HandleTypeDef *pin)
{
    unsigned int value;
    
    //开启ADC
    HAL_ADC_Start(pin);
    
    //读取ADC数据
    value=HAL_ADC_GetValue(pin);
    
    return value*(3.3/4096);
    
}

unsigned int get_adcValue(ADC_HandleTypeDef *pin)
{
    unsigned int value;
    
    //开启ADC
    HAL_ADC_Start(pin);
    
    //读取ADC数据
    value=HAL_ADC_GetValue(pin);
    
    return value;
}
