#include "timer.h"

struct keys key[4]={0,0,0,0};

//��ʱ������жϻص�����
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
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
                    {
                        key[i].judge_sta=1;
                    }
                    break;
                case 1:
                    if(0==key[i].key_sta)
                    {
                        key[i].single_flag=1;
                        key[i].judge_sta=2;
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
}


//Ƶ��
unsigned int frq1=0,frq2=0;

//ռ�ձ�
float duty1=0,duty2=0;

//��ʱ�䣨һ�����ڣ�
double ccrl_val1a=0,ccrl_val2a=0;

//һ�������еĸߵ�ƽ��ռʱ��
unsigned int ccrl_val1b=0,ccrl_val2b=0;

////��ʱ�����벶��ص�����
//void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
//{
//    if(htim->Instance==TIM2)//R40
//    {
//        if(htim->Channel==HAL_TIM_ACTIVE_CHANNEL_1)//ֱ��ͨ��
//        {
//            ccrl_val1a=HAL_TIM_ReadCapturedValue(htim,TIM_CHANNEL_1);
//            ccrl_val1b=HAL_TIM_ReadCapturedValue(htim,TIM_CHANNEL_2);
//               
//             //����ʱ���ļ���ֵ��0
//             __HAL_TIM_SetCounter(htim,0);
//            //����Ƶ��
//            frq1=(80000000/80)/ccrl_val1a;
//            //����ռ�ձ�
//            duty1=(ccrl_val1b/ccrl_val1a)*100;
//            //���¿�����ʱ��,���ö�ʱ����Ӧͨ�������벶����
//            HAL_TIM_IC_Start(htim,TIM_CHANNEL_1);
//            HAL_TIM_IC_Start(htim,TIM_CHANNEL_2);

//        }
//    }
//    if(htim->Instance==TIM3)//R39
//    {
//            ccrl_val2a=HAL_TIM_ReadCapturedValue(htim,TIM_CHANNEL_1);
//            ccrl_val2b=HAL_TIM_ReadCapturedValue(htim,TIM_CHANNEL_2);
//               
//             //����ʱ���ļ���ֵ��0
//             __HAL_TIM_SetCounter(htim,0);
//            //����Ƶ��
//            frq2=(80000000/80)/ccrl_val2a;
//            //����ռ�ձ�
//            duty2=(ccrl_val2b/ccrl_val2a)*100;
//            //���¿�����ʱ�������ö�ʱ����Ӧͨ�������벶����
//            HAL_TIM_IC_Start(htim,TIM_CHANNEL_1);
//            HAL_TIM_IC_Start(htim,TIM_CHANNEL_2);
//    }
//}
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






