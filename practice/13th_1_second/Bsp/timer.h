#ifndef __TIMER_H__
#define __TIMER_H__

#include "main.h"
#include "stdbool.h"
#include "tim.h"

struct keys
{
    bool key_sta;
    unsigned char jugde_sta;
    bool single_flag;
};

extern struct keys key[4];
void keyscan(void);

#endif
