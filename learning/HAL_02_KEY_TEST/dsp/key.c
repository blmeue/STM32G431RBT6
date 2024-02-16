#include "key.h"

//无定时器版本的消抖
unsigned char key_display(void)
{
    unsigned char value=0;
    //B1
    if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_0)==RESET){
        HAL_Delay(10);//延时10毫秒
      if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_0)==RESET){
          value=1;
      }          
    
    }
    //B2
    if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_1)==RESET){
        HAL_Delay(10);//延时10毫秒
      if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_1)==RESET){
          value=2;
      }              
    }
    //B3
    if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_2)==RESET){
         HAL_Delay(10);//延时10毫秒
      if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_2)==RESET){
          value=3;
      }             
    }
    //B4
    if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0)==RESET){
        HAL_Delay(10);//延时10毫秒
      if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0)==RESET){
          value=4;
      }              
    }
    return value;
}
