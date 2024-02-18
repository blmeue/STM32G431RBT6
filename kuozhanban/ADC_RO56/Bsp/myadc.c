#include "myadc.h"

double getADC(ADC_HandleTypeDef *pin)
{
    unsigned int value;
    //开启ADC
    HAL_ADC_Start(pin);
    
    //获取ADC的值
    value=HAL_ADC_GetValue(pin);
    
    //12位精度即4096
    //4096就代表3.3V
    return value*(3.3/4096);
}
