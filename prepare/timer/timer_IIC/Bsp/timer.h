#ifndef __TIMER_H__
#define __TIMER_H__

#include "main.h"
#include "stdbool.h"

struct keys
{
    unsigned char judge_sta;
    bool key_sta;
    bool single_flag;
};

extern struct keys key[4];
//Ƶ��
extern unsigned int frq1,frq2;

//ռ�ձ�
extern float duty1,duty2;

#endif
