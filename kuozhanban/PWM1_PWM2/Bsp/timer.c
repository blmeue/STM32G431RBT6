#include "timer.h"

unsigned int pwm1_frq_ch1=0;//PWM1Ƶ��
unsigned int pwm2_frq_ch2=0;//PWM2Ƶ��

double pwm1_duty_ch1=0;//PWM1ռ�ձ�
double pwm2_duty_ch2=0;//PWM2ռ�ձ�

unsigned int frq_High_ch1=0;//�ߵ�ƽʱ��
unsigned int frq_Low_ch1=0;//�͵�ƽʱ��

unsigned int frq_High_ch2=0;//�ߵ�ƽʱ��
unsigned int frq_Low_ch2=0;//�͵�ƽʱ��

unsigned int uwIC1Value1_T3CH1=0;
unsigned int uwIC1Value2_T3CH1=0;

unsigned int uwIC2Value1_T3CH2=0;
unsigned int uwIC2Value2_T3CH2=0;

uint16_t uhCaptureIndex_T3CH1 = 0;
uint16_t uhCaptureIndex_T3CH2 = 0;

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
    if(htim->Instance==TIM3)
    {
         if(htim->Channel==HAL_TIM_ACTIVE_CHANNEL_1)//ͨ��1
         {
             if(0==uhCaptureIndex_T3CH1)
             {
                 uwIC1Value1_T3CH1=HAL_TIM_ReadCapturedValue(htim,TIM_CHANNEL_1);//��ȡ�����ز���ʱ��
                 __HAL_TIM_SET_CAPTUREPOLARITY(htim,TIM_CHANNEL_1,TIM_ICPOLARITY_FALLING);//���ó��½��ز���
                 uhCaptureIndex_T3CH1=1;
             }
             else if(1==uhCaptureIndex_T3CH1)
             {
                 uwIC1Value2_T3CH1=HAL_TIM_ReadCapturedValue(htim,TIM_CHANNEL_1);//��ȡ�½��ز���ʱ��
                 __HAL_TIM_SET_CAPTUREPOLARITY(htim,TIM_CHANNEL_1,TIM_ICPOLARITY_RISING);//���ó������ز���
                 //����½��ز���ʱ��>�����ز���ʱ��:�ߵ�ƽ����ʱ��=�½��ز���ʱ��-�����ز���ʱ��
                 //���򣬸ߵ�ƽ����ʱ��=��������ֵ-�����ز���ʱ�䣩+�½��ز���ʱ��+1
                 if(uwIC1Value2_T3CH1>uwIC1Value1_T3CH1)
                 {
                     frq_High_ch1=uwIC1Value2_T3CH1-uwIC1Value1_T3CH1;
                 }
                 else if (uwIC1Value2_T3CH1 < uwIC1Value1_T3CH1)
                 {
                     frq_High_ch1=(0xffff-uwIC1Value1_T3CH1)+uwIC1Value2_T3CH1+1;
                 }
                 else
                 {
                     Error_Handler();
                 }
                 uwIC1Value1_T3CH1=uwIC1Value2_T3CH1;//���½��ص�ֵ����uwIC1Value1_T3CH1�����ڼ���͵�ƽ����ʱ��
                 uhCaptureIndex_T3CH1=2;
             }
             else if(2==uhCaptureIndex_T3CH1)
             {
                 uwIC1Value2_T3CH1=HAL_TIM_ReadCapturedValue(htim,TIM_CHANNEL_1);//�����ز���ʱ��
                 //��������ز���ʱ��>�½��ز���ʱ�䣺�͵�ƽ����ʱ��=�����ز���ʱ��-�½��ز���ʱ��
                 //���򣺵͵�ƽ����ʱ��=����װ��ֵ-�½��ز���ʱ�䣩+�����ز���ʱ��+1��
                 if(uwIC1Value2_T3CH1>uwIC1Value1_T3CH1)
                 {
                     frq_Low_ch1=uwIC1Value2_T3CH1-uwIC1Value1_T3CH1;
                 }
                 else if (uwIC1Value2_T3CH1 < uwIC1Value1_T3CH1)
                 {
                     frq_Low_ch1=(0xffff-uwIC1Value1_T3CH1)+uwIC1Value2_T3CH1+1;
                 }
                 else
                 {
                     Error_Handler();
                 }
                 //Ƶ��=(80MHZ/Ԥ��Ƶϵ��)/(�ߵ�ƽʱ��+�͵�ƽʱ��)
                 pwm1_frq_ch1=(80000000/80)/(frq_Low_ch1+frq_High_ch1);
                 //ռ�ձ�=�����ߵ�ƽʱ�䣩/���ߵ�ƽʱ��+�͵�ƽʱ�䣩��*100
                 pwm1_duty_ch1=100.0*frq_High_ch1/(frq_Low_ch1+frq_High_ch1);
                 uhCaptureIndex_T3CH1=0;
             }
         } 
        else if(htim->Channel==HAL_TIM_ACTIVE_CHANNEL_2)//ͨ��2
        {
             if(0==uhCaptureIndex_T3CH2)
             {
                 uwIC2Value1_T3CH2=HAL_TIM_ReadCapturedValue(htim,TIM_CHANNEL_2);//��ȡ�����ز���ʱ��
                 __HAL_TIM_SET_CAPTUREPOLARITY(htim,TIM_CHANNEL_2,TIM_ICPOLARITY_FALLING);//���ó��½��ز���
                 uhCaptureIndex_T3CH2=1;
             }
             else if(1==uhCaptureIndex_T3CH2)
             {
                 uwIC2Value2_T3CH2=HAL_TIM_ReadCapturedValue(htim,TIM_CHANNEL_2);//��ȡ�½��ز���ʱ��
                 __HAL_TIM_SET_CAPTUREPOLARITY(htim,TIM_CHANNEL_2,TIM_ICPOLARITY_RISING);//���ó������ز���
                 //����½��ز���ʱ��>�����ز���ʱ��:�ߵ�ƽ����ʱ��=�½��ز���ʱ��-�����ز���ʱ��
                 //���򣬸ߵ�ƽ����ʱ��=��������ֵ-�����ز���ʱ�䣩+�½��ز���ʱ��+1
                 if(uwIC2Value2_T3CH2>uwIC2Value1_T3CH2)
                 {
                     frq_High_ch2=uwIC2Value2_T3CH2-uwIC2Value1_T3CH2;
                 }
                 else if (uwIC2Value2_T3CH2 < uwIC2Value1_T3CH2)
                 {
                     frq_High_ch2=(0xffff-uwIC2Value1_T3CH2)+uwIC2Value2_T3CH2+1;
                 }
                 else
                 {
                     Error_Handler();
                 }
                 uwIC2Value1_T3CH2=uwIC2Value2_T3CH2;//���½��ص�ֵ����uwIC2Value1_T3CH2�����ڼ���͵�ƽ����ʱ��
                 uhCaptureIndex_T3CH2=2;
             }
             else if(2==uhCaptureIndex_T3CH2)
             {
                 uwIC2Value2_T3CH2=HAL_TIM_ReadCapturedValue(htim,TIM_CHANNEL_2);//�����ز���ʱ��
                 //��������ز���ʱ��>�½��ز���ʱ�䣺�͵�ƽ����ʱ��=�����ز���ʱ��-�½��ز���ʱ��
                 //���򣺵͵�ƽ����ʱ��=����װ��ֵ-�½��ز���ʱ�䣩+�����ز���ʱ��+1��
                 if(uwIC2Value2_T3CH2>uwIC2Value1_T3CH2)
                 {
                     frq_Low_ch2=uwIC2Value2_T3CH2-uwIC2Value1_T3CH2;
                 }
                 else if (uwIC2Value2_T3CH2 < uwIC2Value1_T3CH2)
                 {
                     frq_Low_ch2=(0xffff-uwIC2Value1_T3CH2)+uwIC2Value2_T3CH2+1;
                 }
                 else
                 {
                     Error_Handler();
                 }
                 //Ƶ��=(80MHZ/Ԥ��Ƶϵ��)/(�ߵ�ƽʱ��+�͵�ƽʱ��)
                 pwm2_frq_ch2=(80000000/80)/(frq_Low_ch2+frq_High_ch2);
                 //ռ�ձ�=�����ߵ�ƽʱ�䣩/���ߵ�ƽʱ��+�͵�ƽʱ�䣩��*100
                 pwm2_duty_ch2=100.0*frq_High_ch2/(frq_Low_ch2+frq_High_ch2);
                 uhCaptureIndex_T3CH2=0;
             }
        }
    }
}

