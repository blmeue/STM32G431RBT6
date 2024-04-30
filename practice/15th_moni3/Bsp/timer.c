#include "timer.h"
#include "led.h"

struct key keys[4]={0,0,0,0};

void key_scan(void)
{
    for(int i=0;i<4;i++)
    {
        switch(keys[i].judge_sta)
        {
            case 0:
                if(0==keys[i].key_sta)
                    keys[i].judge_sta=1;
                break;
            case 1:
                if(0==keys[i].key_sta)
                {
                    keys[i].judge_sta=2;
                    keys[i].single_flag=1;
                }
                else
                {
                    keys[i].judge_sta=0;
                }
                break;
            case 2:
                //检测到按键松开
                if(1==keys[i].key_sta)
                {
                    keys[i].judge_sta=0;
                }
                break;
        }
    }
}
 
//产品R37检测通过标志
extern unsigned char R37_Pass;

//产品R38检测通过标志
extern unsigned char R38_Pass;

extern unsigned char ucled;

unsigned int R37_sec=0;
unsigned int R38_sec=0;

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if(htim->Instance==TIM3)
    {
        //读取每个按键的状态
        keys[0].key_sta=HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_0);
        keys[1].key_sta=HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_1);
        keys[2].key_sta=HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_2);
        keys[3].key_sta=HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0);
        key_scan();
        //产品R37检测通过时，LD1点亮1s后熄灭；
        if(1==R37_Pass)
        {
            R37_sec++;
             ucled |=0x01;
            if(100==R37_sec)//1S
            {
                R37_sec=0;
                R37_Pass=0;
                ucled &=0xfe;
            }
        }
        //产品R38检测通过时，LD2点亮1s后熄灭；
        if(1==R38_Pass)
        {
            R38_sec++;
            ucled |=0x02;
            if(100==R38_sec)//1S
            {
                R38_sec=0;
                R38_Pass=0;
                ucled &=0xfd;
            }
        }
    }
}
