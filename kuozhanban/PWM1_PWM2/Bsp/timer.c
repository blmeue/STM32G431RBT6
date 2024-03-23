#include "timer.h"

unsigned int pwm1_frq_ch1=0;//PWM1频率
unsigned int pwm2_frq_ch2=0;//PWM2频率

double pwm1_duty_ch1=0;//PWM1占空比
double pwm2_duty_ch2=0;//PWM2占空比

unsigned int frq_High_ch1=0;//高电平时间
unsigned int frq_Low_ch1=0;//低电平时间

unsigned int frq_High_ch2=0;//高电平时间
unsigned int frq_Low_ch2=0;//低电平时间

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
         if(htim->Channel==HAL_TIM_ACTIVE_CHANNEL_1)//通道1
         {
             if(0==uhCaptureIndex_T3CH1)
             {
                 uwIC1Value1_T3CH1=HAL_TIM_ReadCapturedValue(htim,TIM_CHANNEL_1);//读取上升沿捕获时间
                 __HAL_TIM_SET_CAPTUREPOLARITY(htim,TIM_CHANNEL_1,TIM_ICPOLARITY_FALLING);//设置成下降沿捕获
                 uhCaptureIndex_T3CH1=1;
             }
             else if(1==uhCaptureIndex_T3CH1)
             {
                 uwIC1Value2_T3CH1=HAL_TIM_ReadCapturedValue(htim,TIM_CHANNEL_1);//读取下降沿捕获时间
                 __HAL_TIM_SET_CAPTUREPOLARITY(htim,TIM_CHANNEL_1,TIM_ICPOLARITY_RISING);//设置成上升沿捕获
                 //如果下降沿捕获时间>上升沿捕获时间:高电平保持时间=下降沿捕获时间-上升沿捕获时间
                 //否则，高电平保持时间=（重载载值-上升沿捕获时间）+下降沿捕获时间+1
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
                 uwIC1Value1_T3CH1=uwIC1Value2_T3CH1;//将下降沿的值赋给uwIC1Value1_T3CH1，便于计算低电平保持时间
                 uhCaptureIndex_T3CH1=2;
             }
             else if(2==uhCaptureIndex_T3CH1)
             {
                 uwIC1Value2_T3CH1=HAL_TIM_ReadCapturedValue(htim,TIM_CHANNEL_1);//上升沿捕获时间
                 //如果上升沿捕获时间>下降沿捕获时间：低电平保持时间=上升沿捕获时间-下降沿捕获时间
                 //否则：低电平保持时间=（重装载值-下降沿捕获时间）+上升沿捕获时间+1；
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
                 //频率=(80MHZ/预分频系数)/(高电平时间+低电平时间)
                 pwm1_frq_ch1=(80000000/80)/(frq_Low_ch1+frq_High_ch1);
                 //占空比=（（高电平时间）/（高电平时间+低电平时间））*100
                 pwm1_duty_ch1=100.0*frq_High_ch1/(frq_Low_ch1+frq_High_ch1);
                 uhCaptureIndex_T3CH1=0;
             }
         } 
        else if(htim->Channel==HAL_TIM_ACTIVE_CHANNEL_2)//通道2
        {
             if(0==uhCaptureIndex_T3CH2)
             {
                 uwIC2Value1_T3CH2=HAL_TIM_ReadCapturedValue(htim,TIM_CHANNEL_2);//读取上升沿捕获时间
                 __HAL_TIM_SET_CAPTUREPOLARITY(htim,TIM_CHANNEL_2,TIM_ICPOLARITY_FALLING);//设置成下降沿捕获
                 uhCaptureIndex_T3CH2=1;
             }
             else if(1==uhCaptureIndex_T3CH2)
             {
                 uwIC2Value2_T3CH2=HAL_TIM_ReadCapturedValue(htim,TIM_CHANNEL_2);//读取下降沿捕获时间
                 __HAL_TIM_SET_CAPTUREPOLARITY(htim,TIM_CHANNEL_2,TIM_ICPOLARITY_RISING);//设置成上升沿捕获
                 //如果下降沿捕获时间>上升沿捕获时间:高电平保持时间=下降沿捕获时间-上升沿捕获时间
                 //否则，高电平保持时间=（重载载值-上升沿捕获时间）+下降沿捕获时间+1
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
                 uwIC2Value1_T3CH2=uwIC2Value2_T3CH2;//将下降沿的值赋给uwIC2Value1_T3CH2，便于计算低电平保持时间
                 uhCaptureIndex_T3CH2=2;
             }
             else if(2==uhCaptureIndex_T3CH2)
             {
                 uwIC2Value2_T3CH2=HAL_TIM_ReadCapturedValue(htim,TIM_CHANNEL_2);//上升沿捕获时间
                 //如果上升沿捕获时间>下降沿捕获时间：低电平保持时间=上升沿捕获时间-下降沿捕获时间
                 //否则：低电平保持时间=（重装载值-下降沿捕获时间）+上升沿捕获时间+1；
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
                 //频率=(80MHZ/预分频系数)/(高电平时间+低电平时间)
                 pwm2_frq_ch2=(80000000/80)/(frq_Low_ch2+frq_High_ch2);
                 //占空比=（（高电平时间）/（高电平时间+低电平时间））*100
                 pwm2_duty_ch2=100.0*frq_High_ch2/(frq_Low_ch2+frq_High_ch2);
                 uhCaptureIndex_T3CH2=0;
             }
        }
    }
}

