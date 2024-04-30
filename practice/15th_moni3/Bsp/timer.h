#ifndef __TIMER_H__
#define __TIMER_H__

#include "main.h"
#include "stdbool.h"

struct key
{
    unsigned char judge_sta;
    bool single_flag;
    bool key_sta;
    
};

extern struct key keys[4];

void key_scan(void);


#endif
