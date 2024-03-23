#include "timer.h"
#include "myadc.h"
#include "adc.h"

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
                }
                break;
            case 1:
                if(0==key[i].key_sta)
                {
                    key[i].judge_sta=2;
                    //key[i].single_flag=1;
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
                    
                    if(key[i].sec_sta<=200)
                    {
                        key[i].single_flag=1;
                    }
                    key[i].judge_sta=0;
                    key[i].sec_sta=0;
                }
                else
                {
                    key[i].sec_sta++;
                    if(key[i].sec_sta>200)
                    {
                        key[i].long_flag=1;
                    }
                }
                break;
        }
        
    }
}

extern unsigned char ucled;
extern unsigned char signal;//处于哪个回放状态
extern unsigned char lock;

//记录信号
extern double store_r37;
extern unsigned int store_frq;
extern int store_t;//记录时长
extern double store_duty;

unsigned int store_sec=0;
unsigned int LD1_count=0;
unsigned int LD2_count=0;
unsigned int LD3_count=0;

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if(htim->Instance==TIM4)
    {
        key[0].key_sta=HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_0);
        key[1].key_sta=HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_1);
        key[2].key_sta=HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_2);
        key[3].key_sta=HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0);
        key_scan();
        if(1==lock){
            store_sec++;
            LD1_count++;
            if(store_sec<=(store_t*100))
            {
                //记录频率、占空比、R37输出电压
                store_frq=frq;
                store_duty=duty;
                store_r37=GetAdc(&hadc2);
                if(LD1_count<10)
                {
                    ucled|=0x01;
                }
                else if(LD1_count>=10)
                {
                    ucled&=0xfe;
                    if(20==LD1_count)
                    {
                        LD1_count=0;
                    }
                }
                
            }
            else//记录完成
            {
                lock=0;
                store_sec=0;
                ucled&=0xfe;
                LD1_count=0;
            }
        }
        
        if(1==signal)//回放电压信号,LD2以0.1秒为间隔切换亮、灭
        {
           ucled&=0xfb; 
           store_sec++;
           if(store_sec<=(store_t*100)){
               LD2_count++;
                if(LD2_count<10)
                {
                    ucled|=0x02;
                }
                else
                {
                    ucled&=0xfd;
                    if(20==LD2_count)
                    {
                        LD2_count=0;
                    }
                }
            }
           else//回放完成
           {
               signal=0;
               store_sec=0;
               LD2_count=0;
               ucled&=0xfd;
           }
        }
        else if(2==signal)//回放脉冲信号，LD3亮以0.1秒为间隔切换亮、灭
        {
            ucled&=0xfd;
            store_sec++;
           if(store_sec<=(store_t*100)){
                LD3_count++;
                if(LD3_count<10)
                {
                    ucled|=0x04;
                }
                else
                {
                  ucled&=0xfb;
                  if(20==LD3_count)
                  {
                      LD3_count=0;
                  }
                }
            }
           else//回放完成
           {
             signal=0;
             store_sec=0;
             LD3_count=0;
             ucled&=0xfb;
           }
        }
        else
        {
            ucled&=0xfd;
            ucled&=0xfb;
            LD3_count=0;
            LD2_count=0;
        }
        
    }
}

double duty;
unsigned int frq;

unsigned int frq_h=0;
unsigned int frq_l=0;

unsigned int value1=0;
unsigned int value2=0;
unsigned int value=0;

uint16_t temp;

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
    if(htim->Instance==TIM2)
    {
        if(htim->Channel==HAL_TIM_ACTIVE_CHANNEL_2)
        {
            if(0==temp)
            {
                value1=HAL_TIM_ReadCapturedValue(htim,TIM_CHANNEL_2);
                __HAL_TIM_SET_CAPTUREPOLARITY(htim,TIM_CHANNEL_1,TIM_ICPOLARITY_FALLING);
                temp=1;
            }
            else if(1==temp)
            {
                value2=HAL_TIM_ReadCapturedValue(htim,TIM_CHANNEL_2); 
                __HAL_TIM_SET_CAPTUREPOLARITY(htim,TIM_CHANNEL_1,TIM_ICPOLARITY_RISING);
                 if(value2>value1)
                 {
                     frq_h=value2-value1;
                 }
                 else if(value2<value1)
                 {
                     frq_h=(0xffffffff-value1)+value2+1;
                 }
                 else 
                 {
                     Error_Handler();
                 }
                 value1=value2;
                temp=2;
            }
            else if(2==temp)
            {
                value2=HAL_TIM_ReadCapturedValue(htim,TIM_CHANNEL_2);
                if(value2>value1)
                {
                    frq_l=value2-value1;
                }
                else if(value2<value1)
                {
                    frq_l=(0xffffffff-value1)+value2+1;
                }
                else
                {
                    Error_Handler();
                }
                
                duty=100.0*frq_h/(frq_h+frq_l);
                frq=(800000000/80)/(frq_h+frq_l);
                temp=0;
            }
        }
    }
}
