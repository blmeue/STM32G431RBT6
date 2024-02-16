#include "timer.h"


struct keys key[4]={0,0,0,0};

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    
    //定时器4用来扫描按键
    if(htim->Instance==TIM4)
    {
             key[0].key_sta=HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_0);
             key[1].key_sta=HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_1);
             key[2].key_sta=HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_2);
             key[3].key_sta=HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0);
         for(int i=0;i<4;i++)
        {
           switch(key[i].judge_sta)
            {
                case 0:
                    if(0==key[i].key_sta)
                        key[i].judge_sta=1;
                    break;
                case 1:
                    if(0==key[i].key_sta)//消抖
                    {
                        key[i].single_flag=1;
                        key[i].judge_sta=2;
                    }
                    else 
                        key[i].judge_sta=0;
                    break;
                case 2:
                    if(1==key[i].key_sta)//松开按键
                    {
                        key[i].judge_sta=0;
                    }
                    break;
            }
        }
    }
}


///**********测频率-占空比***********////
double ccrl_val1a=0,ccrl_val2a=0;
unsigned int ccrl_val1b=0,ccrl_val2b=0;

unsigned int frq1=0,frq2=0;//频率

float duty1=0,duty2=0;//占空比

/*
定时器的输入捕获有两个功能，直接捕获模式和间接捕获模式；
直接捕获：只能捕获本身通道的脉冲信号；

间接模式：可以捕获此定时器每个通道的脉信号。

1.捕获频率：初始化定时器基础，开启定时器，开启定时器输入捕获上升沿中断，定时器一直计数，
            直到，捕获到上升沿说明过了一个周期，读取计数值，读取完然后清零，
            等待读取下一个周期，乘以时钟频率，就是周期，然后计算PWM频率。

2.捕获占空比：利用此定时器的另一个通道，作为间接捕获模式，读取下降沿，产生下降沿中断，读取此定时器的计数值，
                就是占空比的高电平时间。


*/




//输入捕获中断回调函数
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)//定时器捕获中断回调函数
{
    //计算定时器2的频率
    if(htim->Instance==TIM2)
    {
        if(htim->Channel==HAL_TIM_ACTIVE_CHANNEL_1)//中断消息来源 选择直接输入的通道
        {
        //保存定时器的计数值
        ccrl_val1a=HAL_TIM_ReadCapturedValue(htim,TIM_CHANNEL_1);
        ccrl_val1b=HAL_TIM_ReadCapturedValue(htim,TIM_CHANNEL_2);//读出的计数值是高电平的计数值
            
        //将定时器的计数值清0
        __HAL_TIM_SetCounter(htim,0);
        //计算频率
        frq1=(80000000/80)/ccrl_val1a;
            
        //计算占空比
         duty1=(ccrl_val1b/ccrl_val1a)*100;
            
        //重新开启定时器
        HAL_TIM_IC_Start(htim,TIM_CHANNEL_1);
        HAL_TIM_IC_Start(htim,TIM_CHANNEL_2);

        }
    }
    
    //计算定时器3的频率
        if(htim->Instance==TIM3)
    {
          if(htim->Channel==HAL_TIM_ACTIVE_CHANNEL_1)//中断消息来源 选择直接输入的通道
        {
        //保存定时器的计数值
        ccrl_val2a=HAL_TIM_ReadCapturedValue(htim,TIM_CHANNEL_1);
        ccrl_val2b=HAL_TIM_ReadCapturedValue(htim,TIM_CHANNEL_2);

        //将定时器的计数值清0
        __HAL_TIM_SetCounter(htim,0);
        //计算频率
        frq2=(80000000/80)/ccrl_val2a;
        
         //计算占空比
         duty2=(ccrl_val2b/ccrl_val2a)*100;   
         
        //重新开启定时器
        HAL_TIM_IC_Start(htim,TIM_CHANNEL_1);
        HAL_TIM_IC_Start(htim,TIM_CHANNEL_2);

        }
    }
}

