#include "interrupt.h"
#include "usart.h"

struct keys key[4]={0,0,0,0};

//中断回调函数
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
 if(htim->Instance==TIM4)//判断是否来自定时器4
 {
     //读取每个按键的按键状态
    key[0].key_sta=HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_0); 
    key[1].key_sta=HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_1); 
    key[2].key_sta=HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_2); 
    key[3].key_sta=HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0); 
    
     for(int i=0;i<4;i++){
         switch(key[i].judge_sta){
             case 0://判断按键是否被按下
                 if(key[i].key_sta==0)//被按下
                 {
                     key[i].key_time=0;
                     key[i].judge_sta=1;
                 }
                 break;
             case 1://消抖
                 if(key[i].key_sta==0)//确实是被按下了
                 { key[i].judge_sta=2;
                 }
                 else
                     key[i].judge_sta=0;
                 break;
             case 2:
                 if(key[i].key_sta==1)//按键被松开,判断松手的过程
                 {
                     key[i].judge_sta=0;
                     if(key[i].key_time<70)
                     {
                     key[i].single_flag=1;//按下标志位置1
                     }
                 }
                 else 
                 {
                     key[i].key_time++;
                     if(key[i].key_time>70)//70毫秒
                     {
                         key[i].long_flag=1;
                     }
                 }
                 break;
         }
     }
 }

}

/************频率测量+占空比***************/

double ccrl_val1a=0,ccrl_val2a=0;
uint ccrl_val1b=0,ccrl_val2b=0;
uint frq1=0,frq2=0;//频率
float duty1=0,duty2=0;//占空比

//中断回调函数
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
 if(htim->Instance==TIM2)//判断是否来自定时器2
 {
     ccrl_val1a=HAL_TIM_ReadCapturedValue(htim,TIM_CHANNEL_1);//直接通道    读取计数值
     ccrl_val1b=HAL_TIM_ReadCapturedValue(htim,TIM_CHANNEL_2);//间接通道    读取计数值

     //清零计数值
     __HAL_TIM_SetCounter(htim,0);
     //计算频率
     frq1=(80000000/80)/ccrl_val1a;//(时钟频率/分频系数)/得到的计数值
     duty1=(ccrl_val1b/ccrl_val1a)*100;
     HAL_TIM_IC_Start(htim,TIM_CHANNEL_1);//重新开启定时器2通道1
     HAL_TIM_IC_Start(htim,TIM_CHANNEL_2);//重新开启定时器2通道2

 }
 
  if(htim->Instance==TIM3)//判断是否来自定时器3
 {
     ccrl_val2a=HAL_TIM_ReadCapturedValue(htim,TIM_CHANNEL_1);//读取计数值
     ccrl_val2b=HAL_TIM_ReadCapturedValue(htim,TIM_CHANNEL_2);//读取计数值

     //清零计数值
     __HAL_TIM_SetCounter(htim,0);
     //计算频率
     frq2=(80000000/80)/ccrl_val2a;//(时钟频率/分频系数)/得到的计数值
     duty2=(ccrl_val2b/ccrl_val2a)*100;
     HAL_TIM_IC_Start(htim,TIM_CHANNEL_1);//重新开启定时器3通道1
     HAL_TIM_IC_Start(htim,TIM_CHANNEL_2);//重新开启定时器3通道2
 }
 
}

char rxdata[30];//最终处理好的数据
uint8_t rxdat;//每次接收的数据
uchar rx_pointer;//确定一下写到哪个位置了

//uart   接收的回调函数
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
    rxdata[rx_pointer++]=rxdat;//保存上一次的数据
    //接收的串口1的数据放在rsxdat
    //最后一个参数是接收位数
    HAL_UART_Receive_IT(&huart1,&rxdat,1);//每次中断只能接收一个字符，启动UART接收中断模式，打开串口接收中断
}


