#include"led.h"


void LED_Disp(uchar dsLED)
{
    //所有引脚置高电平，即所有LED灯灭
    HAL_GPIO_WritePin(GPIOC,GPIO_PIN_All,GPIO_PIN_SET);
    //dsled默认在前八位，左移八位，就可以控制PC15-PC8
    //例如 dsLED=0x65,即0110 0101
    //左移八位就变成 0110 0101 0000 0000
    HAL_GPIO_WritePin(GPIOC,dsLED<<8,GPIO_PIN_RESET);
    //把锁存器打开
    HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_RESET);
    
}
