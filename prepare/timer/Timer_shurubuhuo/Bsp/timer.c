#include "timer.h"


struct keys key[4]={0,0,0,0};

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    
    //��ʱ��4����ɨ�谴��
    if(htim->Instance==TIM4)
    {
        for(int i=0;i<4;i++)
        {
             key[0].key_sta=HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_0);
             key[1].key_sta=HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_1);
             key[2].key_sta=HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_2);
             key[3].key_sta=HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0);
            switch(key[i].judge_sta)
            {
                case 0:
                    if(0==key[i].key_sta)
                        key[i].judge_sta=1;
                    break;
                case 1:
                    if(0==key[i].key_sta)//����
                    {
                        key[i].single_flag=1;
                        key[i].judge_sta=2;
                    }
                    else 
                        key[i].judge_sta=0;
                    break;
                case 2:
                    if(1==key[i].key_sta)//�ɿ�����
                    {
                        key[i].judge_sta=0;
                    }
                    break;
            }
        }
    }
}

unsigned int ccrl_val1=0,ccrl_val2=0;
unsigned int frq1=0,frq2=0;

//���벶���жϻص�����
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)//��ʱ�������жϻص�����
{
    //���㶨ʱ��2��Ƶ��
    if(htim->Instance==TIM2)
    {
        //���涨ʱ���ļ���ֵ
        ccrl_val1=HAL_TIM_ReadCapturedValue(htim,TIM_CHANNEL_1);
        //����ʱ���ļ���ֵ��0
        __HAL_TIM_SetCounter(htim,0);
        //����Ƶ��
        frq1=(80000000/80)/ccrl_val1;
        //���¿�����ʱ��
        HAL_TIM_IC_Start(htim,TIM_CHANNEL_1);
    }
    
    //���㶨ʱ��3��Ƶ��
        if(htim->Instance==TIM3)
    {
        //���涨ʱ���ļ���ֵ
        ccrl_val2=HAL_TIM_ReadCapturedValue(htim,TIM_CHANNEL_1);
        //����ʱ���ļ���ֵ��0
        __HAL_TIM_SetCounter(htim,0);
        //����Ƶ��
        frq2=(80000000/80)/ccrl_val2;
        //���¿�����ʱ��
        HAL_TIM_IC_Start(htim,TIM_CHANNEL_1);
    }
}

