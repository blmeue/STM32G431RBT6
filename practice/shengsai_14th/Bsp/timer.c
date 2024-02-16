#include "timer.h"

unsigned int uwTick_view=0;

struct keys key[4]={0,0,0,0};

unsigned char a;

//��λ��R37�ĵ�ѹ
double R37_v=0;

//��һ���ٶ�ֵ
//���ĳһ��ֵ�������ֵ���ұ�����2S����������ֵ
float old_speed=0;

//���ֵ�ʱ��
unsigned int keep_time=0;

extern unsigned char flag_k2;

//PWM���ģʽ�л�����
unsigned int pwm_n=0;

//pa1��Ƶ��
unsigned int f48=40;

//pa1���Զ���װ��ֵ
unsigned int pa1_frq=200;

//��������ǰPA1��Ƶ��С��200M��Ϊ��Ƶģʽ��־
unsigned char flag_100=0;

//��������ǰPA1Ϊ��Ƶģʽ��־
unsigned char flag_200=0;

//pa1ʵʱռ�ձ�
unsigned int pa1_duty=0;

extern unsigned char ucled;

unsigned char led2_flag=0;//LED2��˸��־

//����/����
unsigned char lock=0;

unsigned int frq=0;
double pa7_frq=0;

unsigned int ccrl_val1a=0;
double ccrl_val2a=0;

 //ʵʱ�ٶ�
float speed_v=0;

//�����ٶ���Ҫ�õ��Ĳ���
unsigned int R=1;
unsigned int K=1;
//��Ƶģʽ�µ��ٶ����ֵ
float H_max=0;
    
//��Ƶģʽ�µ��ٶ����ֵ
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
        
        if(a++==10)//100ms:ÿ0.1��ı�һ�Σ�һ�θı�100HZ
         {
             a=0;
             if(1==flag_k2)
             {
                 uwTick_view=1;//5s��ʱ
                 flag_k2=0;
                  pwm_n++;
                
                 //pa1_frq:PA1����װ��ֵ
                 if(pa1_frq>=200)//��������ǰΪ��Ƶ
                 {
                     flag_100=1;//��������ǰΪ��Ƶ
                 }
                 else if(pa1_frq<=100)//��������ǰΪ��Ƶ
                 {
                     flag_200=1;//��������ǰΪ��Ƶ
                 }
             }
             if(uwTick_view>0&&uwTick_view<=400)
             {
                  
                 if(0==led2_flag)//��
                 {
                     ucled|=0x02;
                     led2_flag=1;
                 }
                 else//��
                 {
                     ucled&=0xfd;
                     led2_flag=0;
                 }
                 led_disp(ucled);
                 //����ǵ�Ƶ,��������ǰΪ��Ƶ
                 //����ʱƵ���������ӣ�ÿ������100����f48++;
                 if(1==flag_100)
                 {
                  //��ʽ��80Mhz/100(Ԥ��Ƶϵ��)/Ƶ��
                  //Ƶ��=80MHZ/Ԥ��Ƶϵ��/��װ��ֵ
                  //Ƶ�ʣ�f48*100;  
                     pa1_frq=(80000000/100)/(100*(++f48));  
                 }
                 //����Ǹ�Ƶ,��������ǰΪ��Ƶ
                 //����ʱƵ��������С��ÿ�μ�С100����f48--
                 if(1==flag_200)
                 {
                     pa1_frq=(80000000/100)/(100*(--f48)); 
                 }
                 //����PWM��Ƶ��
                 //��һ����������ʱ�����
                //�ڶ����������Զ���װ��ֵ(��������)
                 __HAL_TIM_SetAutoreload(&htim2,pa1_frq);
             }
         }
      if(uwTick_view>0)
         {
             uwTick_view++;
             //5S��ʱ������402
             if(uwTick_view>402)
             {
                if(1==flag_100)//��Ƶ
                {
                    flag_100=0; 
                    pa1_frq=100;
                    f48=80;
                }
                
                if(1==flag_200)//��Ƶ
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
         //��Ƶ
       if(100==pa1_frq)
        {
           if((old_speed==speed_v)&&(old_speed>H_max))
            {
                keep_time++;
            }  
            else
            {
                old_speed=speed_v;
                if(keep_time>=200)//������2S
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
                if(keep_time>=200)//������2S
                {
                    L_max=speed_v;
                }
                keep_time=0;
            }
        }
                     R37_v=getadc(&hadc2);
    //����״̬�Ҳ���ģʽ�л���ģʽ�л�ʱռ�ձȲ��ı�
     if((0==lock)&&(0==uwTick_view)){
         //��ȡPA1���Զ���װ��ֵ*ռ�ձȵİٷֱ�
         //�ٽ��ó�����ֵ����ı�ռ�ձȵĺ���
         //PWMռ�ձȣ���ָһ�������ڸߵ�ƽʱ�����ʱ��ı�ֵ��
         //ռ�ձ� = �Ƚ�ֵ(CCR)/�Զ���װ��ֵ(ARR)
         //ռ�ձ� = CCR / (ARR + 1)

        if(R37_v<=1)
        {
            pa1_duty=10;
            //����PWMռ�ձ�
            //��һ����������ʱ�����
            //�ڶ���������PWMͨ����
            //������������CCR���Ƚ�ֵ
            __HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_2,pa1_frq*(pa1_duty/100));
        }
        else if(R37_v>=3)
        {
           pa1_duty=85;//ռ�ձ�,����PWMռ�ձ�
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

        //���㶨ʱ��3��Ƶ��
        if(htim->Instance==TIM3)
    {
        if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2){
        //���涨ʱ���ļ���ֵ
        ccrl_val2a=HAL_TIM_ReadCapturedValue(htim,TIM_CHANNEL_2);
        //ccrl_val2a=__HAL_TIM_GetCounter(htim);
        //����ʱ���ļ���ֵ��0
        __HAL_TIM_SetCounter(htim,0);
        //����Ƶ��
        pa7_frq=(80000000/80)/ccrl_val2a;
        //���¿�����ʱ��
        HAL_TIM_IC_Start(htim,TIM_CHANNEL_2);
        speed_v=(pa7_frq*2*3.14*R)/(100*K);
        }
    }
    
}
 




