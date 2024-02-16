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

extern unsigned int ccrl_val1,ccrl_val2;
extern unsigned int frq1,frq2;
#endif
