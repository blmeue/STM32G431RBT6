#include "timer.h"
#include "led.h"

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
extern unsigned char led3_flag;
extern unsigned char ucled;
unsigned char led3_count=0;

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if(htim->Instance==TIM4)
    {
        key[0].key_sta=HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_0);
        key[1].key_sta=HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_1);
        key[2].key_sta=HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_2);
        key[3].key_sta=HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0); 
        key_scan();  
        if(1==led3_flag)
        {
            led3_count++;
            if(100>led3_count)//¡¡1S
            {
                ucled|=0x04;
            }
            else
            {
                ucled&=0xfb;
                if(200==led3_count)
                {
                    led3_count=0;
                    led3_flag=0;
                }
            }
        }
        else
        {
            ucled&=0xfb;
        }
        led_disp(ucled);
    }
}
