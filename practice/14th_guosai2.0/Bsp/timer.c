#include "timer.h"
#include "tim.h"
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
                    key[i].key_sec=0;
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
                }
                break;
            case 2:
                if(1==key[i].key_sta)
                {
                    
                    key[i].single_flag=1;
                    key[i].judge_sta=0;
                }
                else
                {
                    key[i].key_sec++;
                    if(key[i].key_sec>200)
                    {
                        key[i].long_flag=1;
                    }
                }
                break;
        }
    }
}

extern unsigned char storing;
extern int TT_real;
extern unsigned char ucled;

unsigned int tt_sec;
unsigned int LD1_sec;
unsigned int LD2_sec;
unsigned int LD3_sec;

extern int store_F[1000];
extern double store_D[1000];
extern double store_A[1000];

extern double A;

extern unsigned char store_flag;
unsigned int store_sec=0;

extern unsigned char review;
unsigned int reviewA_sec;
unsigned int reviewF_sec;

unsigned int reviewAA_sec;
unsigned int reviewFF_sec;
extern unsigned char review_flag;
extern unsigned int review_index;
extern unsigned int store_index;

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if(htim->Instance==TIM4)
    {
        key[0].key_sta=HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_0);
        key[1].key_sta=HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_1);
        key[2].key_sta=HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_2);
        key[3].key_sta=HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0);
        keyscan();

        
        //回放脉冲信号
        if(1==review)
        {
            reviewF_sec++;
            reviewFF_sec++;
            LD2_sec++;
            ucled&=0xfb;
            
            reviewA_sec=0;
            
            if(10==reviewFF_sec)
            {
                review_flag=1;
                //review_index++;
                reviewFF_sec=0;
            }
            if(LD2_sec<10)
            {
                ucled|=0x02;
            }
            else
            {
                ucled&=0xfd;
                if(LD2_sec==20)
                {
                    LD2_sec=0;
                }
            }
            if((100*TT_real)==reviewF_sec)
            {
                review=0;
                reviewF_sec=0;
                ucled&=0xfd;
                LD2_sec=0;
               // __HAL_TIM_SetCompare(&htim3,TIM_CHANNEL_2,0);
            }
        }
         //回放电压信号
        if(2==review)
        {
            reviewA_sec++;
            LD3_sec++;
            ucled&=0xfd;
            reviewF_sec=0;
            
            reviewAA_sec++;
            if(10==reviewAA_sec)
            {
                review_flag=2;
                //review_index++;
                reviewAA_sec=0;
            }
            if(LD3_sec<10)
            {
                ucled|=0x04;
            }
            else
            {
                ucled&=0xfb;
                if(LD3_sec==20)
                {
                    LD3_sec=0;
                }
            }
            if((100*TT_real)==reviewA_sec)
            {
                review=0;
                reviewA_sec=0;
                ucled&=0xfb;
                LD3_sec=0;
                //__HAL_TIM_SetCompare(&htim3,TIM_CHANNEL_2,0);
            }
        }
        if(0==review)
        {
            ucled&=0xfd;
            ucled&=0xfb;
            LD3_sec=0;
            LD2_sec=0;
            reviewA_sec=0;
            reviewF_sec=0;
           // review_flag=0;
        }
    }
    if(htim->Instance==TIM6)
    {
        if(1==storing)
         {
            tt_sec++;
            LD1_sec++;
            store_sec++;
           
            if(LD1_sec<10)
            {
                ucled|=0x01;
            }
            else
            {
                ucled&=0xfe;
                if(LD1_sec==20)
                {
                    LD1_sec=0;
                }
            }
            if(10==store_sec)
            {
                store_sec=0;
                store_index++;
                store_flag=1;
            }
            if((100*TT_real)==tt_sec)
            {
                storing=0;
                tt_sec=0;
                ucled&=0xfe;
                LD1_sec=0;
               
            }
        }
         
//        if(1==review&&0==review_flag)
//        {
//            reviewFF_sec++;
//            if(10==reviewFF_sec)
//            {
//                review_flag=1;
//                reviewFF_sec=0;
//            }
//        }
//        else if(2==review&&0==review_flag)
//        {
//            review_sec++;
//            if(10==review_sec)
//            {
//                review_flag=2;
//                review_sec=0;
//            } 
//        }
//        else
//        {
//            review_flag=0;
//            review_sec=0;
//            
//        }
    }
}


double pa1_duty;
unsigned int pa1_frq;
unsigned int pa1_H;
unsigned int pa1_L;
unsigned int value1;
unsigned int value2;
unsigned int value;

unsigned char flag=0;

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
    if(htim->Instance==TIM2)
    {
        if(htim->Channel==HAL_TIM_ACTIVE_CHANNEL_2)
        {
            
            switch(flag)
            {
                case 0:
                    value1=HAL_TIM_ReadCapturedValue(htim,TIM_CHANNEL_2);
                    __HAL_TIM_SET_CAPTUREPOLARITY(htim,TIM_CHANNEL_2,TIM_INPUTCHANNELPOLARITY_FALLING);
                    flag=1;
                    break;
                case 1:
                    value2=HAL_TIM_ReadCapturedValue(htim,TIM_CHANNEL_2);
                    __HAL_TIM_SET_CAPTUREPOLARITY(htim,TIM_CHANNEL_2,TIM_INPUTCHANNELPOLARITY_RISING);
                    if(value2>value1)
                    {
                        pa1_H=value2-value1;
                    }
                    else if(value2<value1)
                    {
                        pa1_H=0xffffffff-value2+value1;
                    }
                    else
                    {
                        Error_Handler();
                    }
                    value1=value2;
                    flag=2;
                    break;
                case 2:
                    value2=HAL_TIM_ReadCapturedValue(htim,TIM_CHANNEL_2);
                    if(value2>value1)
                    {
                        pa1_L=value2-value1;
                    }
                    else if(value2<value1)
                    {
                        pa1_L=0xffffffff-value2+value1;
                    }
                    else
                    {
                        Error_Handler();
                    }
                    pa1_frq=(80000000/80)/(pa1_L+pa1_H);
                    pa1_duty=(100.0*pa1_H)/(pa1_L+pa1_H);
                    flag=0;
                    break;
            }
            
        }
    }
}

