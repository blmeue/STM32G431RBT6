#include "myadc.h"

//读取adc的值
//参数是adc的引脚
double getADC(ADC_HandleTypeDef *pin)
{
    uint32_t adc;
    HAL_ADC_Start(pin);//开启ADC
    
    //读取ADC值
    adc=HAL_ADC_GetValue(pin);
    
    //精度为12位，即4096
    //电源电压为3.3V
    //3.3V就代表4096
    //电压的取值范围是0-3.3V
    //具体的电压值
    return adc*(3.3/4096);
}
