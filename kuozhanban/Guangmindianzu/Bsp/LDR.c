#include "LDR.h"


unsigned char trdo;//��������ɼ��������
double trao;//ģ������ɼ��������

void LDR_ReadAODO(void)
{
    trdo=HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_3);
   // trao=adc2_in17_AO1;
}
