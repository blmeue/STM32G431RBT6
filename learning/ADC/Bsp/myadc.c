#include "myadc.h"

//��ȡadc��ֵ
//������adc������
double getADC(ADC_HandleTypeDef *pin)
{
    uint32_t adc;
    HAL_ADC_Start(pin);//����ADC
    
    //��ȡADCֵ
    adc=HAL_ADC_GetValue(pin);
    
    //����Ϊ12λ����4096
    //��Դ��ѹΪ3.3V
    //3.3V�ʹ���4096
    //��ѹ��ȡֵ��Χ��0-3.3V
    //����ĵ�ѹֵ
    return adc*(3.3/4096);
}
