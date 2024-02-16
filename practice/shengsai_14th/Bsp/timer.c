#include "timer.h"

unsigned int uwTick_view=0;

struct keys key[4]={0,0,0,0};

unsigned char a;

//电位器R37的电压
double R37_v=0;

//上一个速度值
//如果某一个值大于最大值并且保持了2S，则更新最大值
float old_speed=0;

//保持的时间
unsigned int keep_time=0;

extern unsigned char flag_k2;

//PWM输出模式切换次数
unsigned int pwm_n=0;

//pa1的频率
unsigned int f48=40;

//pa1的自动重装载值
unsigned int pa1_frq=200;

//按键按下前PA1的频率小于200M，为低频模式标志
unsigned char flag_100=0;

//按键按下前PA1为高频模式标志
unsigned char flag_200=0;

//pa1实时占空比
unsigned int pa1_duty=0;

extern unsigned char ucled;

unsigned char led2_flag=0;//LED2闪烁标志

//解锁/锁定
unsigned char lock=0;

unsigned int frq=0;
double pa7_frq=0;

unsigned int ccrl_val1a=0;
double ccrl_val2a=0;

 //实时速度
float speed_v=0;

//测量速度需要用到的参数
unsigned int R=1;
unsigned int K=1;
//高频模式下的速度最大值
float H_max=0;
    
//低频模式下的速度最大值
float L_max=0;

void key_scan(void)
{
     for(int i=0;i<4;i++)
    {
        switch(key[i].judge_sta)
        {
            case 0:
                if(0==key[i].key_sta)
                {
                    key[i].key_time=0;
                    key[i].judge_sta=1;
                }
                break;
            case 1:
                if(0==key[i].key_sta)
                {
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
                    if(key[i].key_time<200)
                    {
                        key[i].long_single=0;
                        key[i].single_flag=1;
                    }
                    else
                    {
                        key[i].long_single=1;
                        key[i].single_flag=1;
                    }
                    key[i].key_time=0;
                }
                else
                {
                    key[i].key_time++;
                    if(key[i].key_time>=200)
                    {
                        key[i].long_single=1;
                        key[i].single_flag=1;
                    }                       
                }
                break;
            
        }
    } 
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
       
    if(htim->Instance==TIM4)
    {
        
        key[0].key_sta=HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_0);
        key[1].key_sta=HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_1);
        key[2].key_sta=HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_2);
        key[3].key_sta=HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0);

        key_scan();
        
        if(a++==10)//100ms:每0.1秒改变一次，一次改变100HZ
         {
             a=0;
             if(1==flag_k2)
             {
                 uwTick_view=1;//5s计时
                 flag_k2=0;
                  pwm_n++;
                
                 //pa1_frq:PA1的重装载值
                 if(pa1_frq>=200)//按键按下前为低频
                 {
                     flag_100=1;//按键按下前为低频
                 }
                 else if(pa1_frq<=100)//按键按下前为高频
                 {
                     flag_200=1;//按键按下前为高频
                 }
             }
             if(uwTick_view>0&&uwTick_view<=400)
             {
                  
                 if(0==led2_flag)//亮
                 {
                     ucled|=0x02;
                     led2_flag=1;
                 }
                 else//灭
                 {
                     ucled&=0xfd;
                     led2_flag=0;
                 }
                 led_disp(ucled);
                 //如果是低频,按键按下前为低频
                 //按下时频率慢慢增加，每次增加100，即f48++;
                 if(1==flag_100)
                 {
                  //公式：80Mhz/100(预分频系数)/频率
                  //频率=80MHZ/预分频系数/重装载值
                  //频率：f48*100;  
                     pa1_frq=(80000000/100)/(100*(++f48));  
                 }
                 //如果是高频,按键按下前为高频
                 //按下时频率慢慢减小，每次减小100，即f48--
                 if(1==flag_200)
                 {
                     pa1_frq=(80000000/100)/(100*(--f48)); 
                 }
                 //调节PWM的频率
                 //第一个参数：定时器句柄
                //第二个参数：自动重装载值(设置周期)
                 __HAL_TIM_SetAutoreload(&htim2,pa1_frq);
             }
         }
      if(uwTick_view>0)
         {
             uwTick_view++;
             //5S定时器超过402
             if(uwTick_view>402)
             {
                if(1==flag_100)//低频
                {
                    flag_100=0; 
                    pa1_frq=100;
                    f48=80;
                }
                
                if(1==flag_200)//高频
                 {
                     flag_200=0;
                     pa1_frq=200;
                     f48=40;
                 }
             }
            
             if(uwTick_view>=500)
             {
                 uwTick_view=0;
                 ucled=ucled&0xfd;
                 led_disp(ucled);
             }
         }
         //高频
       if(100==pa1_frq)
        {
           if((old_speed==speed_v)&&(old_speed>H_max))
            {
                keep_time++;
            }  
            else
            {
                old_speed=speed_v;
                if(keep_time>=200)//保持了2S
                {
                    H_max=speed_v;
                }
                keep_time=0;
            }
        }
        if(200==pa1_frq)
        {
           if((old_speed==speed_v)&&(speed_v>L_max))
            {
                keep_time++;
            }  
            else
            {
                old_speed=speed_v;
                if(keep_time>=200)//保持了2S
                {
                    L_max=speed_v;
                }
                keep_time=0;
            }
        }
                     R37_v=getadc(&hadc2);
    //解锁状态且不是模式切换，模式切换时占空比不改变
     if((0==lock)&&(0==uwTick_view)){
         //获取PA1的自动重装载值*占空比的百分比
         //再将得出来的值带入改变占空比的函数
         //PWM占空比：是指一个周期内高电平时间和总时间的比值。
         //占空比 = 比较值(CCR)/自动重装载值(ARR)
         //占空比 = CCR / (ARR + 1)

        if(R37_v<=1)
        {
            pa1_duty=10;
            //设置PWM占空比
            //第一个参数：定时器句柄
            //第二个参数：PWM通道数
            //第三个参数：CCR，比较值
            __HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_2,pa1_frq*(pa1_duty/100));
        }
        else if(R37_v>=3)
        {
           pa1_duty=85;//占空比,设置PWM占空比
             __HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_2,pa1_frq*(pa1_duty/100));
        }
        else
        {
            pa1_duty=37.5*R37_v-27.5;
            __HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_2,pa1_frq*(pa1_duty/100));
        }
      }
   }
}



void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{

        //计算定时器3的频率
        if(htim->Instance==TIM3)
    {
        if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2){
        //保存定时器的计数值
        ccrl_val2a=HAL_TIM_ReadCapturedValue(htim,TIM_CHANNEL_2);
        //ccrl_val2a=__HAL_TIM_GetCounter(htim);
        //将定时器的计数值清0
        __HAL_TIM_SetCounter(htim,0);
        //计算频率
        pa7_frq=(80000000/80)/ccrl_val2a;
        //重新开启定时器
        HAL_TIM_IC_Start(htim,TIM_CHANNEL_2);
        speed_v=(pa7_frq*2*3.14*R)/(100*K);
        }
    }
    
}
 




