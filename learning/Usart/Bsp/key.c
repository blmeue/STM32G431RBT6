#include "key.h"

unsigned char key_scan(void)
{
    //减速函数会实现消抖功能，因此在检测按键按下时不需要调用延时函数进行消抖
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
