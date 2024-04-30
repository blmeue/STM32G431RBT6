#include "timer.h"

struct keys key[4]={0,0,0,0};

void keyscan(void)
{
    for(int i=0;i<4;i++)
    {
        switch(key[i].jugde_sta)
        {
            case 0:
                if(0==key[i].key_sta)
                {
                    key[i].jugde_sta=1;
                }
                break;
            case 1:
                if(0==key[i].key_sta)
                {
                    key[i].jugde_sta=2;
                    key[i].single_flag=1;
                }
                else
                {
                    key[i].jugde_sta=0;
                }
                break;
            case 2:
                if(1==key[i].key_sta)
                {
                    key[i].jugde_sta=0;
                }
                break;
        }
    }
}

extern unsigned char ucled;
extern unsigned char success_flag;//密码正确标志位
extern int lose_count;//输错次数
extern int F;
extern int D;
extern unsigned char view;
extern unsigned char Input_B1;
extern unsigned char Input_B2;
extern unsigned char Input_B3;

unsigned int success_sec=0;
unsigned int lose_sec=0;
unsigned int LD2_sec;

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if(htim->Instance==TIM4)
    {
        key[0].key_sta=HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_0);
        key[1].key_sta=HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_1);
        key[2].key_sta=HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_2);
        key[3].key_sta=HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0);
        keyscan();
        //LED1亮5秒，PA1输出2Khz,10%占空比的脉冲信号
        //5秒后，PA1恢复输出2Khz,50%的占空比
        //且5秒后界面切回到密码输入界面，屏幕娴熟的三位密码值重置为字符@
        if(1==success_flag)
        {
            success_sec++;
            ucled|=0x01;
            F=2000;
            D=10;
            //设置比较值
            //占空比=输出比较值/重装载值
            __HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_2,10);
        
            //设置预分频系数
             //频率=（80MHZ）/（预分频系数+1）/（重装载值+1）
            __HAL_TIM_SET_PRESCALER(&htim2,(80000000/2000.0/100.0));
            
            if(500==success_sec)
            {
                ucled&=0xfe;
                success_flag=0;
                success_sec=0;
                Input_B1='@';
                Input_B2='@';
                Input_B3='@';
                F=1000;
                D=50;
                view=0;
                
            //设置比较值
            //占空比=输出比较值/重装载值
            __HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_2,50);
        
            //设置预分频系数
             //频率=（80MHZ）/（预分频系数+1）/（重装载值+1）
            __HAL_TIM_SET_PRESCALER(&htim2,(80000000/1000.0/100.0));
                            
            }
            
        }
        
        //连续输入三次以上的密码，LD2以0.1秒为间隔亮灭，5秒后熄灭
        if(lose_count>=3)
        {
            lose_sec++;
            LD2_sec++;
            if(LD2_sec<10)
            {
                ucled|=0x02;
            }
            else
            {
                ucled&=0xfd;
                if(20==LD2_sec)
                {
                    LD2_sec=0;
                }
            }
            if(500==lose_sec)
            {
                ucled&=0xfd;
                lose_sec=0;
                lose_count=0;
            }
        }
        else
        {
            ucled&=0xfd;
        }
        
    }
}

