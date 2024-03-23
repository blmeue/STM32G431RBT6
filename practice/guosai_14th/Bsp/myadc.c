#include "myadc.h"

double GetAdc(ADC_HandleTypeDef *pin)
{
    unsigned int value;
    HAL_ADC_Start(pin);
    value=HAL_ADC_GetValue(pin);
    
    return value*3.3/4096.0;
}
