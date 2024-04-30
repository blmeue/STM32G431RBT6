#include "timer.h"

struct keys key[4]={0,0,0,0};

void keyscan(void)
{
    for(int i=0;i<4;i++)
    {
        switch(key[i].judge_sta)
        {
            case 0:
                if(0==key[i].key_sta)
                {
                    key[i].judge_sta=1;
                }            
                break;
            case 1:
                if(0==key[i].key_sta)
                {
                    key[i].judge_sta=2;
                    key[i].single_flag=1;
                }
                else
                {
                    key[i].judge_sta=0;
                }
                break;
            case 2:
                if(1==key[i].key_sta)
                {
                    key[i].judge_sta=0;
                }
                break;
        }
    }
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if(htim->Instance==TIM4)
    {
        key[0].key_sta=HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_0);
        key[1].key_sta=HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_1);
        key[2].key_sta=HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_2);
        key[3].key_sta=HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0);
        keyscan();
    }
}

unsigned int pa1_frq=0;
double value=0;

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
    if(htim->Instance==TIM2)
    {
      if(htim->Channel==HAL_TIM_ACTIVE_CHANNEL_2)
      {
        value=HAL_TIM_ReadCapturedValue(htim,TIM_CHANNEL_2);
        
        __HAL_TIM_SetCounter(htim,0);
        
        HAL_TIM_IC_Start(htim,TIM_CHANNEL_2);
        
        pa1_frq=(80000000/80)/value;
      } 
    }
}

