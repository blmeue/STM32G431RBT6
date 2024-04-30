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
                    key[i].key_sec=0;//开始计时
                    key[i].judge_sta=1;
                }
                break;
            case 1:
                if(0==key[i].key_sta)
                {
                    key[i].judge_sta=2;
                    key[i].key_sec++;
                }
                else
                {
                    key[i].judge_sta=0;;
                    key[i].key_sec=0;
                }
                break;
            case 2:
                if(1==key[i].key_sta)//按键松开
                {
                    if(key[i].key_sec>200)//长按按键超过两秒
                    {
                        key[i].long_flag=1;
                    }
                    else
                    {
                        key[i].single_flag=1;
                    }
                    key[i].key_sec=0;
                    key[i].judge_sta=0;
                }
                else
                {
                    key[i].key_sec++;
                }
                break;
        }
    }
}

extern unsigned char choose_mode;
extern unsigned char ucled;
unsigned int choose_mode_sec=0;
unsigned int LD2_sec;
extern unsigned char M;

unsigned int pa1_psc=0;
extern unsigned int pa1_frq;

extern double MH;//高频模式最大速度
extern double ML;//高频模式最小速度
extern double v_speed;
extern unsigned char M;
extern unsigned int N;


double old_speed;

unsigned int MH_sec=0;
unsigned int ML_sec=0;

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if(htim->Instance==TIM4)
    {
        key[0].key_sta=HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_0);
        key[1].key_sta=HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_1);
        key[2].key_sta=HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_2);
        key[3].key_sta=HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0);
        key_scan();
        
        if(1==choose_mode)
        {
            if(0==choose_mode_sec)
            {
                if(0==M)
                {
                    pa1_frq=4000;
                }
                else
                {
                    pa1_frq=8000;
                }                
            }
            choose_mode_sec++;
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
            
            //低频模式转变为高频模式:4KHZ增加到8KHZ,即5S需要增加4000HZ,每秒增加800HZ,每10毫秒增加8HZ
            if(0==M)
            {
                pa1_frq+=8;
                pa1_psc=(80000000.0/100.0/pa1_frq)-1;
                __HAL_TIM_SET_PRESCALER(&htim2,pa1_psc);
                if(8000==pa1_frq)
                {
                    M=1;
                    N++;
                    choose_mode=0;
                    choose_mode_sec=0;
                    ucled&=0xfd; 
                }
            }
            //高频模式转变为低频模式
            else if(1==M)
            {
                pa1_frq-=8;
                pa1_psc=(80000000.0/100.0/pa1_frq)-1;
                __HAL_TIM_SET_PRESCALER(&htim2,pa1_psc);
                if(4000==pa1_frq)
                {
                    M=0;
                    N++;
                    choose_mode=0;
                    choose_mode_sec=0;
                    ucled&=0xfd; 
                }
            }
//            //频率=（80MHZ/(预分频系数+1)）/(重装载值+1)
//            pa1_psc=(80000000.0/100.0/pa1_frq)-1;
//            __HAL_TIM_SET_PRESCALER(&htim2,pa1_psc);
//            
//            //if(500==choose_mode_sec)
//            if((8000==pa1_frq||4000==pa1_frq)&&(choose_mode_sec>=500))
//            {
//                M=!M;
//                N++;
//                choose_mode=0;
//                choose_mode_sec=0;
//                ucled&=0xfd; 
//            }
        }
        else
        {
            ucled&=0xfd;           
        }
                
        if(1==M)
        {
            if(fabs(old_speed-v_speed)<=0.5&&v_speed>MH)
            {
                MH_sec++;
            }
            else
            {
                old_speed=v_speed;
                if(MH_sec>=200)
                    MH=v_speed;
                MH_sec=0;
            }
        }
        else if(0==M)
        {
            if(fabs(old_speed-v_speed)<=0.5&&v_speed>ML)
            {
                ML_sec++;
            }
            else
            {
                old_speed=v_speed;
                if(ML_sec>=200)
                    ML=v_speed;
                ML_sec=0;
            }
        }
        
    }
}

unsigned int pa7_frq=0;
double value=0;
extern unsigned int R;
extern unsigned int K;

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
    if(htim->Instance==TIM3)
    {
        if(htim->Channel==HAL_TIM_ACTIVE_CHANNEL_2)
            {
        value=HAL_TIM_ReadCapturedValue(htim,TIM_CHANNEL_2);
        
        __HAL_TIM_SetCounter(htim,0);
        pa7_frq=(80000000/80)/value;
        
        HAL_TIM_IC_Start(htim,TIM_CHANNEL_2);
         v_speed=(pa7_frq*2*3.14*R)/(100*K);
        }
    }

}


