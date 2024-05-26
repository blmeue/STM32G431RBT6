#include "timer.h"

struct keys key[4]={0,0,0,0};

void keyscan(void)
{
    for(int i=0;i<4;i++)
    {
        switch(key[i].judge_key)
        {
            case 0:
                if(0==key[i].key_sta)
                {
                    key[i].key_sec=0;
                    key[i].judge_key=1;
                }
                break;
            case 1:
                if(0==key[i].key_sta)
                {
                    key[i].judge_key=2;
                    key[i].single_flag=1;
                }
                else
                {
                    key[i].judge_key=0;
                }
                break;
            case 2:
                if(1==key[i].key_sta)
                {
                    key[i].judge_key=0;
                    //按下超过一秒后松开
                    if(key[i].key_sec>100)
                    {
                        key[i].long_flag=1;
                    }
                }
                else
                {
                    key[i].key_sec++;
                }
                
                break;
        }
    }
}

extern unsigned char ucled;
unsigned char LD3_sec;
extern unsigned char LD3_flag;

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if(htim->Instance==TIM4)
    {
        key[0].key_sta=HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_0);
        key[1].key_sta=HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_1);
        key[2].key_sta=HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_2);
        key[3].key_sta=HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0);
        keyscan();
        
        if(1==LD3_flag)
        {
            LD3_sec++;
            if(LD3_sec<10)
            {
                ucled|=0x04;
            }
            else
            {
                ucled&=0xfb;
                if(LD3_sec==20)
                    LD3_sec=0;
            }
        }
        else
        {
            ucled&=0xfb;
        }
        
    }
}

unsigned int pa1_frq;
unsigned int pa1_value=0;

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
    if(htim->Instance==TIM2)
    {
        if(htim->Channel==HAL_TIM_ACTIVE_CHANNEL_2)
        {
            pa1_value=HAL_TIM_ReadCapturedValue(htim,TIM_CHANNEL_2);
            pa1_frq=(80000000/80)/pa1_value;
           if(pa1_frq<100)
               pa1_frq=100;
           if(pa1_frq>10000)
               pa1_frq=10000; 
            __HAL_TIM_SetCounter(htim,0);
            HAL_TIM_IC_Start_IT(htim,TIM_CHANNEL_2);

        }
    }
}

