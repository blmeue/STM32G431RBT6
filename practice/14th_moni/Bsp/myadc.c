#include "myadc.h"

double get_Adc(ADC_HandleTypeDef *pin)
{
    int value;
    
    HAL_ADC_Start(pin);
    
    value=HAL_ADC_GetValue(pin);
    
    return value*(3.3/4096);
}
