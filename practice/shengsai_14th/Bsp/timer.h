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
    bool single_flag;//�̰�
    bool long_single;//����
    unsigned int key_time;//����ʱ��
};

extern struct keys key[4];
extern unsigned int frq;
extern double ccrl_val2a;
extern double pa7_frq;


//pa1��Ƶ��
extern unsigned int f48;

//pa1����װ��ֵ
extern unsigned int pa1_frq;

//pa1ʵʱռ�ձ�
extern unsigned int pa1_duty;

//��λ��R37�ĵ�ѹ
extern double R37_v;

//����/����
extern unsigned char lock;

//PWM���ģʽ�л�����
extern unsigned int pwm_n;

 //ʵʱ�ٶ�
extern float speed_v;

//�����ٶ���Ҫ�õ��Ĳ���
extern unsigned int R;
extern unsigned int K;

//��Ƶģʽ�µ��ٶ����ֵ
extern float H_max;
    
//��Ƶģʽ�µ��ٶ����ֵ
extern float L_max;

extern unsigned int uwTick_view;


void key_scan(void);


#endif
