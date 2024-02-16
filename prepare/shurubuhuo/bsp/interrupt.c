#include "interrupt.h"

struct keys key[4]={0,0,0,0};

//�жϻص�����
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
 if(htim->Instance==TIM4)//�ж��Ƿ����Զ�ʱ��4
 {
     //��ȡÿ�������İ���״̬
    key[0].key_sta=HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_0); 
    key[1].key_sta=HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_1); 
    key[2].key_sta=HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_2); 
    key[3].key_sta=HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0); 
    
     for(int i=0;i<4;i++){
         switch(key[i].judge_sta){
             case 0://�жϰ����Ƿ񱻰���
                 if(key[i].key_sta==0)//������
                 {
                     key[i].key_time=0;
                     key[i].judge_sta=1;
                 }
                 break;
             case 1://����
                 if(key[i].key_sta==0)//ȷʵ�Ǳ�������
                 { key[i].judge_sta=2;
                 }
                 else
                     key[i].judge_sta=0;
                 break;
             case 2:
                 if(key[i].key_sta==1)//�������ɿ�,�ж����ֵĹ���
                 {
                     key[i].judge_sta=0;
                     if(key[i].key_time<70)
                     {
                     key[i].single_flag=1;//���±�־λ��1
                     }
                 }
                 else 
                 {
                     key[i].key_time++;
                     if(key[i].key_time>70)//70����
                     {
                         key[i].long_flag=1;
                     }
                 }
                 break;
         }
     }
 }

}

uint ccrl_val1=0,ccrl_val2=0;
uint frq1=0,frq2=0;

//�жϻص�����
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
 if(htim->Instance==TIM2)//�ж��Ƿ����Զ�ʱ��1
 {
     ccrl_val1=HAL_TIM_ReadCapturedValue(htim,TIM_CHANNEL_1);//��ȡ����ֵ
     //�������ֵ
     __HAL_TIM_SetCounter(htim,0);
     //����Ƶ��
     frq1=(80000000/80)/ccrl_val1;//(ʱ��Ƶ��/��Ƶϵ��)/�õ��ļ���ֵ
     HAL_TIM_IC_Start(htim,TIM_CHANNEL_1);//���¿�����ʱ��2
 }
 
  if(htim->Instance==TIM3)//�ж��Ƿ����Զ�ʱ��1
 {
     ccrl_val2=HAL_TIM_ReadCapturedValue(htim,TIM_CHANNEL_1);//��ȡ����ֵ
     //�������ֵ
     __HAL_TIM_SetCounter(htim,0);
     //����Ƶ��
     frq2=(80000000/80)/ccrl_val2;//(ʱ��Ƶ��/��Ƶϵ��)/�õ��ļ���ֵ
     HAL_TIM_IC_Start(htim,TIM_CHANNEL_1);//���¿�����ʱ��2
 }
 
}




