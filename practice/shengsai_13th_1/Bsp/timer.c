#include "timer.h"
#include "String.h"
#include "lcd.h"
#include "tim.h"
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
extern unsigned char ucled;
unsigned int keep_time;//LED1亮,输出信号界面持续时间
extern unsigned char view;
extern char mima_in[4];

unsigned int led2_count=0;
unsigned int ms_count=0;

//连续输入三个以上错误密码的标志
extern unsigned char flag;
//led2亮灭标志
unsigned char led2_flag=0;

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if(htim->Instance==TIM4)
    {
        key[0].key_sta=HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_0);
        key[1].key_sta=HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_1);
        key[2].key_sta=HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_2);
        key[3].key_sta=HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0);
        key_scan();
        
        if(1==view)
        {
            keep_time++;
            if(keep_time<500)
            {
                ucled|=0x01;
               //PA1 输出 2KHz 10%占空比的脉冲信号
               //频率=80MHZ/预分频系数/重装载值 即重装载值=80MHZ/预分频系数/频率
                //pa1=(80000000)/80/2000
                unsigned int pa1;
                pa1=80000000/80/2000;
                __HAL_TIM_SetAutoreload(&htim2,pa1-1);//设置重装载值
                
                //占空比=比较输出/自动重装载值
                __HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_2,pa1*10/100);//设置比较输出值
               // HAL_TIM_GenerateEvent(&htim2, TIM_EVENTSOURCE_UPDATE);
            }
            else
            {
                ucled&=0xfe;
                //屏幕显示的三位密码值重置为字符@。
                memset(mima_in,'@',sizeof(mima_in)-1);
                
                //切换为 1KHz 方波信号输出
                unsigned int pa1;
                pa1=80000000/80/1000;
                __HAL_TIM_SetAutoreload(&htim2,pa1-1);//设置重装载值
                
                //占空比=比较输出/自动重装载值
                __HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_2,pa1*10/100);//设置比较输出值

                view=0;
                LCD_Clear(Black);
                keep_time=0;
            }
        }
        
        //连续 3 次以上的密码输入错误操作（含 3 次），
        if(1==flag)
        {
            led2_count++;
            ms_count++;
          if(led2_count<500){
               if((0==led2_flag)&&(10==ms_count))//0.1S
               {
                   ucled|=0x02;
                   led2_flag=1;
                   ms_count=0;
               }  
                else if((1==led2_flag)&&(10==ms_count))
                {
                    ucled&=0xfd;
                    led2_flag=0;
                    ms_count=0;
                }
            }
          else
          {
              led2_count=0;
              ucled&=0xfd;
              flag=0;
          }
        }
        else
        {
            ucled&=0xfd;
        }
        led_disp(ucled);
    }
}
