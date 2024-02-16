#include "badc.h"

//读取adc值
//参数为 adc的引脚
double getADC(ADC_HandleTypeDef *pin){
    uint adc;//读出来的原始值
    HAL_ADC_Start(pin);//开启ADC
    
    //读取ADC值
    adc=HAL_ADC_GetValue(pin);
    
    //电源电压：3.3V
    //精度为12位：即4096
    //电压的取值范围是0-3.3V
    //3.3V就代表4096
    return adc*3.3/4096;//具体的电压值
    
}
