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

extern unsigned char ucled;
extern unsigned char view;
extern unsigned char buy_flag;

extern unsigned int X_rep;
extern unsigned int Y_rep;

unsigned int buy_sec=0;
unsigned int LD2_sec=0;

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if(htim->Instance==TIM4)
    {
        key[0].key_sta=HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_0);
        key[1].key_sta=HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_1);
        key[2].key_sta=HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_2);
        key[3].key_sta=HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0);
        keyscan();
        //确认购买,购买界面下
        if(1==buy_flag)
        {
            buy_sec++;
           __HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_2,30); 
            //LD1点亮5秒后熄灭
            ucled|=0x01;
            //PA1频率保持2KHZ，但占空比变为30%，5秒后恢复5%
            //占空比=比较值/重装载值
            
            if(500==buy_sec)
            {
                buy_flag=0;
                ucled&=0xfe;
                buy_sec=0; 
                __HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_2,5);
                
            }
        }
        
        //X,Y库存数量都为0,LD2以0.1秒为间隔切换亮灭状态
        if((0==X_rep)&&(0==Y_rep))
        {
            LD2_sec++;
            if(LD2_sec<10)
            {
                ucled|=0x02;
            }
            else
            {
                ucled&=0xfd;
                if(20==LD2_sec)
                    LD2_sec=0;
            }
        }
        else
        {
            ucled&=0xfd;
        }
    }
}

