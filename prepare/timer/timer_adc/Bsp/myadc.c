#include "myadc.h"

//��ȡADC��ֵ
//������ADC������
double get_adc(ADC_HandleTypeDef *pin)
{
    //��������ԭʼֵ
    unsigned int adc;
    //����adc
    HAL_ADC_Start(pin);
    
    //��ȡADC��ֵ
    adc=HAL_ADC_GetValue(pin);
    
    //12λ���ȼ�4096
    //4096�ʹ���3.3V
    return adc*(3.3/4096);
    
}
