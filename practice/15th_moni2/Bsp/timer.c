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
                    key[i].judge_sta=1;
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
                    //key[i].single_flag=1;
                    key[i].judge_sta=0;
                    
                }
                break;
        }
    }
}
unsigned int uwTick_sleep=0;//记录时间
extern unsigned char usart_led;
unsigned int usart_sec=0;
extern unsigned char ucled;

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if(htim->Instance==TIM4)
    {
        key[0].key_sta=HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_0);
        key[1].key_sta=HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_1);
        key[2].key_sta=HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_2);
        key[3].key_sta=HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0);
        key_scan();
        uwTick_sleep++;
        if(usart_led!=0)
        {
            usart_sec++;
            if(usart_sec<=300)//亮三秒
            {
                ucled|=0x08;  
            }
            else
            {
                ucled&=0xf7;
                usart_sec=0;
                usart_led=0;
            }
           // led_disp(ucled);
        }
        else
        {
            ucled&=0xf7;
        }
    }
}
