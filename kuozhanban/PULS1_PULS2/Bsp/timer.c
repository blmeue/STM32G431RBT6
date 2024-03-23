#include "timer.h"

unsigned int PUSL1_frq_T2CH2=0;
unsigned int PUSL2_frq_T2CH3=0;

uint32_t uwIC2Value1_T2CH2 = 0;//��һ�β��������ص�ʱ��
uint32_t uwIC2Value2_T2CH2 = 0;//�ڶ��β��������ص�ʱ��
uint32_t uwDiffCapture_T2CH2 = 0;//һ����������ʱ��

uint32_t uwIC3Value1_T2CH3 = 0;//��һ�β��������ص�ʱ��
uint32_t uwIC3Value2_T2CH3 = 0;//�ڶ��β��������ص�ʱ��
uint32_t uwDiffCapture_T2CH3 = 0;//һ����������ʱ��

uint16_t uhCaptureIndex_T2CH2 = 0;
uint16_t uhCaptureIndex_T2CH3 = 0;


void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
    if(htim->Instance==TIM2)
    {
        if(htim->Channel==HAL_TIM_ACTIVE_CHANNEL_2)
        {
            if(0==uhCaptureIndex_T2CH2)
            {
               uwIC2Value1_T2CH2=HAL_TIM_ReadCapturedValue(htim,TIM_CHANNEL_2);//��ȡ��һ�β��������ص�ʱ��
               uhCaptureIndex_T2CH2=1;
            }
            else if(1==uhCaptureIndex_T2CH2)
            {
                uwIC2Value2_T2CH2=HAL_TIM_ReadCapturedValue(htim,TIM_CHANNEL_2);//��ȡ�ڶ��β��������ص�ʱ��
                if(uwIC2Value2_T2CH2>uwIC2Value1_T2CH2)//һ��������ռʱ��=�ڶ��β���ʱ��-��һ�β���ʱ��
                {
                    uwDiffCapture_T2CH2=uwIC2Value2_T2CH2-uwIC2Value1_T2CH2;
                }
                else if(uwIC2Value2_T2CH2<uwIC2Value1_T2CH2)//һ��������ռʱ��=����װ��ֵ-��һ�β���ʱ�䣩+�ڶ��β���ʱ��
                {
                    uwDiffCapture_T2CH2=(0xffffff-uwIC2Value1_T2CH2)+uwIC2Value2_T2CH2;
                }
                else
                {
                    Error_Handler();
                }
                //Ƶ��=��80MHz/Ԥ��Ƶϵ����/��һ��������ռʱ�䣩
                PUSL1_frq_T2CH2=(80000000/80)/uwDiffCapture_T2CH2;
                uhCaptureIndex_T2CH2=0;
            }
        }
        else if(htim->Channel==HAL_TIM_ACTIVE_CHANNEL_3)
        {
             if(0==uhCaptureIndex_T2CH3)
            {
               uwIC3Value1_T2CH3=HAL_TIM_ReadCapturedValue(htim,TIM_CHANNEL_3);//��ȡ��һ�β��������ص�ʱ��
               uhCaptureIndex_T2CH3=1;
            }
            else if(1==uhCaptureIndex_T2CH3)
            {
                uwIC3Value2_T2CH3=HAL_TIM_ReadCapturedValue(htim,TIM_CHANNEL_3);//��ȡ�ڶ��β��������ص�ʱ��
                if(uwIC3Value2_T2CH3>uwIC3Value1_T2CH3)//һ��������ռʱ��=�ڶ��β���ʱ��-��һ�β���ʱ��
                {
                    uwDiffCapture_T2CH3=uwIC3Value2_T2CH3-uwIC3Value1_T2CH3;
                }
                else if(uwIC3Value2_T2CH3<uwIC3Value1_T2CH3)//һ��������ռʱ��=����װ��ֵ-��һ�β���ʱ�䣩+�ڶ��β���ʱ��
                {
                    uwDiffCapture_T2CH3=(0xffffff-uwIC3Value1_T2CH3)+uwIC3Value2_T2CH3;
                }
                else
                {
                    Error_Handler();
                }
                //Ƶ��=��80MHz/Ԥ��Ƶϵ����/��һ��������ռʱ�䣩
                PUSL2_frq_T2CH3=(80000000/80)/uwDiffCapture_T2CH3;
                uhCaptureIndex_T2CH3=0;
            }           
        }
    }
}
