#include "timer.h"

struct keys key[4]={0,0,0,0};

void key_scan(void)
{
    for(int i=0;i<4;i++)
    {
        switch(key[i].judge_sta)
        {
            case 0:
                if(0==key[i].key_sta)
                {
                    key[i].judge_sta=1;
                    key[i].sec_sta=0;
                    key[i].single_flag=0;
                    key[i].long_flag=0;
                }
                break;
            case 1:
                if(0==key[i].key_sta)
                {
                    key[i].judge_sta=2;
                }
                else
                {
                    key[i].judge_sta=0;
                    key[i].sec_sta=0;
                }
                break;
            case 2:
                if(1==key[i].key_sta)//按键松开
                {
                    if(key[i].sec_sta<=100)
                    {
                        key[i].single_flag=1;
                    }
                    else//超过1秒，长按
                    {
                        key[i].long_flag=1;
                        key[i].single_flag=0;
                    }
                    key[i].sec_sta=0;
                    key[i].judge_sta=0;
                }
                else
                {
                    key[i].sec_sta++;
                }
                break;
        }
    }
}

extern unsigned char ucled;
extern unsigned char LD3_flag;
unsigned int LD3_sec=0;

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if(htim->Instance==TIM4)
    {
        key[0].key_sta=HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_0);
        key[1].key_sta=HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_1);
        key[2].key_sta=HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_2);
        key[3].key_sta=HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0);
        key_scan();
        if(1==LD3_flag)
        {
           LD3_sec++;
           if(LD3_sec<=100)
           {
               ucled|=0x04;
           }
           else
           {
               ucled&=0xfb;
               if(200==LD3_sec)
               {
                   LD3_sec=0;
               }
           }
        }
        else
        {
           ucled&=0xfb; 
        }
    }
}

unsigned int frq=0;
unsigned int value1=0,value2=0;
unsigned int value=0;
unsigned char value_temp=0;

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
    if(htim->Instance==TIM2)
    {
        if(htim->Channel==HAL_TIM_ACTIVE_CHANNEL_2)
        {
            switch(value_temp)
            {
                case 0:
                    value1=HAL_TIM_ReadCapturedValue(htim,TIM_CHANNEL_2);
                    value_temp=1;
                    break;
                case 1:
                    value2=HAL_TIM_ReadCapturedValue(htim,TIM_CHANNEL_2);
                    if(value2>value1)
                    {
                        value=value2-value1;
                    }
                    else if(value2<value1)
                    {
                        value=(0xffffffff-value1)+value2;
                    }
                    else
                    {
                        Error_Handler();
                    }
                    frq=(80000000/80)/value;
                    value_temp=0;
                    break;
            }
        }
    }
}

