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
                    key[i].judge_sta=1;
                }
                break;
            case 1:
                if(0==key[i].key_sta)
                {
                    key[i].judge_sta=2;
                    key[i].single_flag=1;
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

unsigned int L_sec=0;//����������Ϊ��ת��L��ʱ,��ʱ
unsigned int R_sec=0;//����������Ϊ��ת��L��ʱ����ʱ

extern unsigned char N;//��������
extern unsigned char view;
extern unsigned char ucled;

unsigned int L_led=0;//�ڳ�����ʻ���ݽ����£�����������Ϊ��ת��L��ʱ��ָʾ��LD1��0.1��Ϊ����л�����״̬������Ϩ��
unsigned int R_led=0;//�ڳ�����ʻ���ݽ����£�����������Ϊ��ת��R��ʱ��ָʾ��LD2��0.1��Ϊ����л�����״̬������Ϩ��

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if(htim->Instance==TIM4)
    {
        key[0].key_sta=HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_0);
        key[1].key_sta=HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_1);
        key[2].key_sta=HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_2);
        key[3].key_sta=HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0);
        key_scan();
        if(2==N&&view==0)//��ת
        {
            //R_sec=0;
            L_sec++;
            L_led++;
            if(L_led<10)//��0.1s
            {
                ucled&=0xfd;
                ucled|=0x01;
            }
            else//��0.1S
            {
                ucled&=0xfe;
                if(L_led==20)
                {
                    L_led=0;
                }
            }
        }
        else if(1==N&&view==0)
        {
            //L_sec=0;
            R_sec++;
            R_led++;
            if(R_led<10)
            {
                ucled&=0xfe;
                ucled|=0x02;
            }
            else
            {
                ucled&=0xfd;
                if(R_led==20)
                {
                    R_led=0;
                }
            }
        }
        else
        {
             //L_sec=0;
             //R_sec=0;
             R_led=0;
             L_led=0;
             ucled&=0xfd;
             ucled&=0xfe;
        }
        
    }
}
