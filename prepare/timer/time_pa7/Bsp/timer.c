#include "timer.h"

struct keys key[4]={0,0,0,0};

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if(htim->Instance==TIM2 )
    {
        //��ȡÿ��������״̬
        key[0].key_sta=HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_0);
        key[1].key_sta=HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_1);
        key[2].key_sta=HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_2);
        key[3].key_sta=HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0);
        
        for(int i=0;i<4;i++)
        {
            switch(key[i].judge_sta)
            {
                case 0:
                    if(key[i].key_sta==0)//������
                    {
                        key[i].judge_sta=1;
                    }
                    break;
                case 1://����
                    if(key[i].key_sta==0)
                    {
                        key[i].single_flag=1;
                        key[i].judge_sta=2;
                    }
                    else
                        key[i].judge_sta=0;
                    break;
                case 2:
                    if(key[i].key_sta==1)//���֣��������ɿ�,�ж����ֵĹ���
                        key[i].judge_sta=0;
                    break;
            }
        }
    }
}

/* ���´�����ӵ�task.c�� */
 
#define PI 3.14
 
uint16_t prescaler = 1-1;
uint32_t ccl_value;
uint32_t pa7_frq;
unsigned int R=1;
unsigned int K=1;
float V=0;
 
/* ����Ƶ�� */
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance==TIM3 && htim->Channel==HAL_TIM_ACTIVE_CHANNEL_2)
    {
        ccl_value = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_2);
        __HAL_TIM_SetCounter(&htim3, 0);
        pa7_frq = 80000000/((prescaler+1)*ccl_value);
        V = (pa7_frq*2*PI*R)/(100*K);
        HAL_TIM_IC_Start(htim, TIM_CHANNEL_2);
    }
}
