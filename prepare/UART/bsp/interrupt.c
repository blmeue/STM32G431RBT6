#include "interrupt.h"
#include "usart.h"

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

/************Ƶ�ʲ���+ռ�ձ�***************/

double ccrl_val1a=0,ccrl_val2a=0;
uint ccrl_val1b=0,ccrl_val2b=0;
uint frq1=0,frq2=0;//Ƶ��
float duty1=0,duty2=0;//ռ�ձ�

//�жϻص�����
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
 if(htim->Instance==TIM2)//�ж��Ƿ����Զ�ʱ��2
 {
     ccrl_val1a=HAL_TIM_ReadCapturedValue(htim,TIM_CHANNEL_1);//ֱ��ͨ��    ��ȡ����ֵ
     ccrl_val1b=HAL_TIM_ReadCapturedValue(htim,TIM_CHANNEL_2);//���ͨ��    ��ȡ����ֵ

     //�������ֵ
     __HAL_TIM_SetCounter(htim,0);
     //����Ƶ��
     frq1=(80000000/80)/ccrl_val1a;//(ʱ��Ƶ��/��Ƶϵ��)/�õ��ļ���ֵ
     duty1=(ccrl_val1b/ccrl_val1a)*100;
     HAL_TIM_IC_Start(htim,TIM_CHANNEL_1);//���¿�����ʱ��2ͨ��1
     HAL_TIM_IC_Start(htim,TIM_CHANNEL_2);//���¿�����ʱ��2ͨ��2

 }
 
  if(htim->Instance==TIM3)//�ж��Ƿ����Զ�ʱ��3
 {
     ccrl_val2a=HAL_TIM_ReadCapturedValue(htim,TIM_CHANNEL_1);//��ȡ����ֵ
     ccrl_val2b=HAL_TIM_ReadCapturedValue(htim,TIM_CHANNEL_2);//��ȡ����ֵ

     //�������ֵ
     __HAL_TIM_SetCounter(htim,0);
     //����Ƶ��
     frq2=(80000000/80)/ccrl_val2a;//(ʱ��Ƶ��/��Ƶϵ��)/�õ��ļ���ֵ
     duty2=(ccrl_val2b/ccrl_val2a)*100;
     HAL_TIM_IC_Start(htim,TIM_CHANNEL_1);//���¿�����ʱ��3ͨ��1
     HAL_TIM_IC_Start(htim,TIM_CHANNEL_2);//���¿�����ʱ��3ͨ��2
 }
 
}

char rxdata[30];//���մ���õ�����
uint8_t rxdat;//ÿ�ν��յ�����
uchar rx_pointer;//ȷ��һ��д���ĸ�λ����

//uart   ���յĻص�����
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
    rxdata[rx_pointer++]=rxdat;//������һ�ε�����
    //���յĴ���1�����ݷ���rsxdat
    //���һ�������ǽ���λ��
    HAL_UART_Receive_IT(&huart1,&rxdat,1);//ÿ���ж�ֻ�ܽ���һ���ַ�������UART�����ж�ģʽ���򿪴��ڽ����ж�
}


