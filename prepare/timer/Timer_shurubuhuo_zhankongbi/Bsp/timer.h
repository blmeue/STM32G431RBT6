#ifndef __TIMER_H__
#define __TIMER_H__


#include "main.h"
#include "stdbool.h"

struct keys 
{
    unsigned char judge_sta;
    bool single_flag;
    bool key_sta;
};

extern struct keys key[4];

extern double ccrl_val1a,ccrl_val2a;
extern unsigned int ccrl_val1b,ccrl_val2b;

extern unsigned int frq1,frq2;//ÆµÂÊ

extern float duty1,duty2;//Õ¼¿Õ±È
#endif
