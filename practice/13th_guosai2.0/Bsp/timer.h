#ifndef __TIMER_H__
#define __TIMER_H__

#include "main.h"
#include "stdbool.h"

struct keys
{
    bool key_sta;
    unsigned char judge_key;
    bool single_flag;
    bool long_flag;
    int key_sec;
};

extern struct keys key[4];
extern unsigned int pa1_frq;


void keyscan(void);

#endif
