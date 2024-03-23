#include "myadc.h"

double getADC(ADC_HandleTypeDef *pin)
{
    unsigned int value;
    //����ADC
    HAL_ADC_Start(pin);
    
    //��ȡADC��ֵ
    value=HAL_ADC_GetValue(pin);
    
    //12λ���ȼ�4096
    //4096�ʹ���3.3V
    return value*(3.3/4096);
}
