#include "key.h"

unsigned char key_scan(void)
{
    unsigned char num=0;
    if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_0)==RESET)
    {
        num=1;
    }
    if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_1)==RESET)
    {
        num=2;
    }
    if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_2)==RESET)
    {
        num=3;
    }
    if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0)==RESET)
    {
        num=4;
    }
    return num;
}

