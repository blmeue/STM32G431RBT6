#include "interrupt.h"

struct keys key[4]={0,0,0,0};

//中断回调函数
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
 if(htim->Instance==TIM3)//判断是否来自定时器3
 {
     //读取每个按键的按键状态
    key[0].key_sta=HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_0); 
    key[1].key_sta=HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_1); 
    key[2].key_sta=HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_2); 
    key[3].key_sta=HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0); 
    
     for(int i=0;i<4;i++){
         switch(key[i].judge_sta){
             case 0://判断按键是否被按下
                 if(key[i].key_sta==0)//被按下
                 {
                     key[i].key_time=0;
                     key[i].judge_sta=1;
                 }
                 break;
             case 1://消抖
                 if(key[i].key_sta==0)//确实是被按下了
                 { 
                     key[i].judge_sta=2;
                 }
                 else
                     key[i].judge_sta=0;
                 break;
             case 2:
                 if(key[i].key_sta==1)//按键被松开,判断松手的过程
                 {
                     key[i].judge_sta=0;
                     if(key[i].key_time<70)
                     {
                     key[i].single_flag=1;//按下标志位置1
                     }
                 }
                 else 
                 {
                     key[i].key_time++;
                     if(key[i].key_time>70)//700毫秒
                     {
                         key[i].long_flag=1;
                     }
                 }
                 break;
         }
     }
 }

}
