#include "myusart.h"

void Get_adc(ADC_HandleTypeDef *pin,double *v)
{
      int value;
    for(int i=0;i<2;i++)
    {
        HAL_ADC_Start(pin);
        //等待转换完成
        HAL_ADC_PollForConversion(pin,50);
        value=HAL_ADC_GetValue(pin);
        v[i]=value*3.3/4096.0;
    }
    
    HAL_ADC_Stop(pin);
}
