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
extern unsigned int uwTick_sleep;//��¼ʱ��

void key_scan(void);


#endif
