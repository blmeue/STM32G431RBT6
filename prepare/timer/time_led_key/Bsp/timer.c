#include "timer.h"

struct keys key[4]={0,0,0,0};

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if(htim->Instance==TIM2 )
    {
        //读取每个按键的状态
        key[0].key_sta=HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_0);
        key[1].key_sta=HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_1);
        key[2].key_sta=HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_2);
        key[3].key_sta=HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0);
        
        for(int i=0;i<4;i++)
        {
            switch(key[i].judge_sta)
            {
                case 0:
                    if(key[i].key_sta==0)//被按下
                    {
                        key[i].judge_sta=1;
                    }
                    break;
                case 1://消抖
                    if(key[i].key_sta==0)
                    {
                        key[i].single_flag=1;
                        key[i].judge_sta=2;
                    }
                    else
                        key[i].judge_sta=0;
                    break;
                case 2:
                    if(key[i].key_sta==1)//松手，按键被松开,判断松手的过程
                        key[i].judge_sta=0;
                    break;
            }
        }
    }
}
