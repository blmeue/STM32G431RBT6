#include "interrupt.h"

struct keys key[4]={0,0,0,0};

//�жϻص�����
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
 if(htim->Instance==TIM3)//�ж��Ƿ����Զ�ʱ��3
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
                     key[i].judge_sta=1;
                 break;
             case 1://����
                 if(key[i].key_sta==0)//ȷʵ�Ǳ�������
                 { key[i].judge_sta=2;
                   key[i].single_flag=1;//���±�־λ��1
                 }
                 else
                     key[i].judge_sta=0;
                 break;
             case 2:
                 if(key[i].key_sta==1)//�������ɿ�,�ж����ֵĹ���
                 {
                     key[i].judge_sta=0;
                 }
                 break;
         }
     }
 }

}