#ifndef __TIMER_H__
#define __TIMER_H__


#include "main.h"
#include "stdbool.h"
#include "tim.h"
#include "math.h"

struct keys
{
    bool key_sta;
    unsigned char judge_sta;
    bool single_flag;
    bool long_flag;
    unsigned int key_sec;
};

extern struct keys key[4];
extern unsigned int pa7_frq;

void key_scan(void);


#endif
