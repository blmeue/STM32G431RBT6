#ifndef __TIMER_H__
#define __TIMER_H__

#include "main.h"
#include "stdbool.h"
#include "myadc.h"
#include "adc.h"
#include "tim.h"
#include "led.h"

struct keys
{
    unsigned char judge_sta;
    bool key_sta;
    bool single_flag;//短按
    bool long_single;//长按
    unsigned int key_time;//按下时间
};

extern struct keys key[4];
extern unsigned int frq;
extern double ccrl_val2a;
extern double pa7_frq;


//pa1的频率
extern unsigned int f48;

//pa1的重装载值
extern unsigned int pa1_frq;

//pa1实时占空比
extern unsigned int pa1_duty;

//电位器R37的电压
extern double R37_v;

//解锁/锁定
extern unsigned char lock;

//PWM输出模式切换次数
extern unsigned int pwm_n;

 //实时速度
extern float speed_v;

//测量速度需要用到的参数
extern unsigned int R;
extern unsigned int K;

//高频模式下的速度最大值
extern float H_max;
    
//低频模式下的速度最大值
extern float L_max;

extern unsigned int uwTick_view;


void key_scan(void);


#endif
