#include "timer.h"


struct keys key[4]={0,0,0,0};

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    
    //��ʱ��4����ɨ�谴��
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


///**********��Ƶ��-ռ�ձ�***********////
double ccrl_val1a=0,ccrl_val2a=0;
unsigned int ccrl_val1b=0,ccrl_val2b=0;

unsigned int frq1=0,frq2=0;//Ƶ��

float duty1=0,duty2=0;//ռ�ձ�

/*
��ʱ�������벶�����������ܣ�ֱ�Ӳ���ģʽ�ͼ�Ӳ���ģʽ��
ֱ�Ӳ���ֻ�ܲ�����ͨ���������źţ�

���ģʽ�����Բ���˶�ʱ��ÿ��ͨ�������źš�

1.����Ƶ�ʣ���ʼ����ʱ��������������ʱ����������ʱ�����벶���������жϣ���ʱ��һֱ������
            ֱ��������������˵������һ�����ڣ���ȡ����ֵ����ȡ��Ȼ�����㣬
            �ȴ���ȡ��һ�����ڣ�����ʱ��Ƶ�ʣ��������ڣ�Ȼ�����PWMƵ�ʡ�

2.����ռ�ձȣ����ô˶�ʱ������һ��ͨ������Ϊ��Ӳ���ģʽ����ȡ�½��أ������½����жϣ���ȡ�˶�ʱ���ļ���ֵ��
                ����ռ�ձȵĸߵ�ƽʱ�䡣


*/




//���벶���жϻص�����
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)//��ʱ�������жϻص�����
{
    //���㶨ʱ��2��Ƶ��
    if(htim->Instance==TIM2)
    {
        if(htim->Channel==HAL_TIM_ACTIVE_CHANNEL_1)//�ж���Ϣ��Դ ѡ��ֱ�������ͨ��
        {
        //���涨ʱ���ļ���ֵ
        ccrl_val1a=HAL_TIM_ReadCapturedValue(htim,TIM_CHANNEL_1);
        ccrl_val1b=HAL_TIM_ReadCapturedValue(htim,TIM_CHANNEL_2);//�����ļ���ֵ�Ǹߵ�ƽ�ļ���ֵ
            
        //����ʱ���ļ���ֵ��0
        __HAL_TIM_SetCounter(htim,0);
        //����Ƶ��
        frq1=(80000000/80)/ccrl_val1a;
            
        //����ռ�ձ�
         duty1=(ccrl_val1b/ccrl_val1a)*100;
            
        //���¿�����ʱ��
        HAL_TIM_IC_Start(htim,TIM_CHANNEL_1);
        HAL_TIM_IC_Start(htim,TIM_CHANNEL_2);

        }
    }
    
    //���㶨ʱ��3��Ƶ��
        if(htim->Instance==TIM3)
    {
          if(htim->Channel==HAL_TIM_ACTIVE_CHANNEL_1)//�ж���Ϣ��Դ ѡ��ֱ�������ͨ��
        {
        //���涨ʱ���ļ���ֵ
        ccrl_val2a=HAL_TIM_ReadCapturedValue(htim,TIM_CHANNEL_1);
        ccrl_val2b=HAL_TIM_ReadCapturedValue(htim,TIM_CHANNEL_2);

        //����ʱ���ļ���ֵ��0
        __HAL_TIM_SetCounter(htim,0);
        //����Ƶ��
        frq2=(80000000/80)/ccrl_val2a;
        
         //����ռ�ձ�
         duty2=(ccrl_val2b/ccrl_val2a)*100;   
         
        //���¿�����ʱ��
        HAL_TIM_IC_Start(htim,TIM_CHANNEL_1);
        HAL_TIM_IC_Start(htim,TIM_CHANNEL_2);

        }
    }
}

