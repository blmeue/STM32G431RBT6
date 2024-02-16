#include "key.h"

unsigned char key_scan(void)
{
    unsigned char keynum=0;
    if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_0)==RESET)
    {
        keynum=1;
    }
    if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_1)==RESET)
    {
        keynum=2;
    }
    if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_2)==RESET)
    {
        keynum=3;
    }
    if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0)==RESET)
    {
        keynum=4;
    }
    return keynum;
}
