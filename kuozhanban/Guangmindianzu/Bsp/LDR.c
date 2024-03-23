#include "LDR.h"


unsigned char trdo;//数字输出采集储存参数
double trao;//模拟输出采集储存参数

void LDR_ReadAODO(void)
{
    trdo=HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_3);
   // trao=adc2_in17_AO1;
}
