#include "badc.h"

//��ȡadcֵ
//����Ϊ adc������
double getADC(ADC_HandleTypeDef *pin){
    uint adc;//��������ԭʼֵ
    HAL_ADC_Start(pin);//����ADC
    
    //��ȡADCֵ
    adc=HAL_ADC_GetValue(pin);
    
    //��Դ��ѹ��3.3V
    //����Ϊ12λ����4096
    //��ѹ��ȡֵ��Χ��0-3.3V
    //3.3V�ʹ���4096
    return adc*3.3/4096;//����ĵ�ѹֵ
    
}
