#include "myadc.h"

//读取ADC的值
//参数是ADC的引脚
double get_adc(ADC_HandleTypeDef *pin)
{
    //读出来的原始值
    unsigned int adc;
    //开启adc
    HAL_ADC_Start(pin);
    
    //读取ADC的值
    adc=HAL_ADC_GetValue(pin);
    
    //12位精度即4096
    //4096就代表3.3V
    return adc*(3.3/4096);
    
}
