#include "timer.h"
#include "led.h"
#include "tim.h"

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
                if(1==key[i].key_sta)//按键松开
                {
                    key[i].judge_sta=0;
                }
                break;
        }
    }
}

extern unsigned char ucled;
extern unsigned char view;
//商品X\Y的库存数量
extern unsigned int X_reserve;
extern unsigned int Y_reserve;

unsigned int LED1_count;
unsigned int LED2_count;

//B4按键按下标志
extern unsigned char B4_flag;

unsigned int count=0;

//商品X、Y库存数量均为0标志
extern unsigned char Reserve_0_flag;

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if(htim->Instance==TIM3)
    {
        key[0].key_sta=HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_0);
        key[1].key_sta=HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_1);
        key[2].key_sta=HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_2);
        key[3].key_sta=HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0);
        key_scan();
        
        //在购买界面下，按下B4按键确认购买后，LD1点亮5秒后熄灭
        if((0==view)&&(1==B4_flag))
        {
           ucled|=0x01;
           LED1_count++;
           //占空比=比较输出/自动重装载值；本程序中设置PWM的自动重装载值为100
            //调用__HAL_TIM_SetCompare可以重新设置占空比
            //第一个参数：定时器句柄
            //第二个参数：PWM通道数
            //第三个参数：CCR，比较值
            __HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_2,30);
           if(LED1_count>=500)//5秒
           {
               ucled&=0xfe;
               __HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_2,5);
               LED1_count=0;
               B4_flag=0;
           }
        }
        else 
        {
            B4_flag=0;
            ucled&=0xfe;
            __HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_2,5);
        }
        
        //商品X、Y库存数量均为0
        if(1==Reserve_0_flag)
        {
            count++;
            if((count>0)&&(count<10))
            {
                ucled|=0x02;
            }
            else
            {
                ucled&=0xfd;
                if(count==20)
                    count=0;
            }
        }
        else
        {
            ucled &=0xfd;
        }
        
        led_disp(ucled);
    }
}
