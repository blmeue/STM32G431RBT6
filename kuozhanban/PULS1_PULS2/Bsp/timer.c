#include "timer.h"

unsigned int PUSL1_frq_T2CH2=0;
unsigned int PUSL2_frq_T2CH3=0;

uint32_t uwIC2Value1_T2CH2 = 0;//第一次捕获上升沿的时间
uint32_t uwIC2Value2_T2CH2 = 0;//第二次捕获上升沿的时间
uint32_t uwDiffCapture_T2CH2 = 0;//一个周期所用时间

uint32_t uwIC3Value1_T2CH3 = 0;//第一次捕获上升沿的时间
uint32_t uwIC3Value2_T2CH3 = 0;//第二次捕获上升沿的时间
uint32_t uwDiffCapture_T2CH3 = 0;//一个周期所用时间

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
               uwIC2Value1_T2CH2=HAL_TIM_ReadCapturedValue(htim,TIM_CHANNEL_2);//获取第一次捕获上升沿的时间
               uhCaptureIndex_T2CH2=1;
            }
            else if(1==uhCaptureIndex_T2CH2)
            {
                uwIC2Value2_T2CH2=HAL_TIM_ReadCapturedValue(htim,TIM_CHANNEL_2);//获取第二次捕获上升沿的时间
                if(uwIC2Value2_T2CH2>uwIC2Value1_T2CH2)//一个周期所占时间=第二次捕获时间-第一次捕获时间
                {
                    uwDiffCapture_T2CH2=uwIC2Value2_T2CH2-uwIC2Value1_T2CH2;
                }
                else if(uwIC2Value2_T2CH2<uwIC2Value1_T2CH2)//一个周期所占时间=（重装载值-第一次捕获时间）+第二次捕获时间
                {
                    uwDiffCapture_T2CH2=(0xffffff-uwIC2Value1_T2CH2)+uwIC2Value2_T2CH2;
                }
                else
                {
                    Error_Handler();
                }
                //频率=（80MHz/预分频系数）/（一个周期所占时间）
                PUSL1_frq_T2CH2=(80000000/80)/uwDiffCapture_T2CH2;
                uhCaptureIndex_T2CH2=0;
            }
        }
        else if(htim->Channel==HAL_TIM_ACTIVE_CHANNEL_3)
        {
             if(0==uhCaptureIndex_T2CH3)
            {
               uwIC3Value1_T2CH3=HAL_TIM_ReadCapturedValue(htim,TIM_CHANNEL_3);//获取第一次捕获上升沿的时间
               uhCaptureIndex_T2CH3=1;
            }
            else if(1==uhCaptureIndex_T2CH3)
            {
                uwIC3Value2_T2CH3=HAL_TIM_ReadCapturedValue(htim,TIM_CHANNEL_3);//获取第二次捕获上升沿的时间
                if(uwIC3Value2_T2CH3>uwIC3Value1_T2CH3)//一个周期所占时间=第二次捕获时间-第一次捕获时间
                {
                    uwDiffCapture_T2CH3=uwIC3Value2_T2CH3-uwIC3Value1_T2CH3;
                }
                else if(uwIC3Value2_T2CH3<uwIC3Value1_T2CH3)//一个周期所占时间=（重装载值-第一次捕获时间）+第二次捕获时间
                {
                    uwDiffCapture_T2CH3=(0xffffff-uwIC3Value1_T2CH3)+uwIC3Value2_T2CH3;
                }
                else
                {
                    Error_Handler();
                }
                //频率=（80MHz/预分频系数）/（一个周期所占时间）
                PUSL2_frq_T2CH3=(80000000/80)/uwDiffCapture_T2CH3;
                uhCaptureIndex_T2CH3=0;
            }           
        }
    }
}
