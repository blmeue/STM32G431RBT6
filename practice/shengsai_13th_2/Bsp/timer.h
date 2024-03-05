#ifndef __TIMER_H__
#define __TIMER_H__

#include "main.h"
#include "stdbool.h"

struct keys
{
    bool key_sta;
    unsigned char judge_sta;
    bool single_flag;
};


extern struct keys key[4];

void key_scan(void);

#endif
