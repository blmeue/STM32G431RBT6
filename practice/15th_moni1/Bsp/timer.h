#ifndef __TIMER_H__
#define __TIMER_H__

#include "main.h"
#include "stdbool.h"

struct keys
{
    bool key_sta;
    bool single_flag;
    unsigned char judge_sta;
};

extern struct keys key[4];

extern unsigned int L_sec;//当导航方向为左转（L）时,计时
extern unsigned int R_sec;//当导航方向为左转（L）时，计时


void key_scan(void);


#endif
