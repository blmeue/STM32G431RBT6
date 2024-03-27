#ifndef __TIMER_H__
#define __TIMER_H__


#include "main.h"
#include "stdbool.h"

struct keys{
    bool key_sta;
    bool single_flag;
    bool long_flag;
    unsigned char judge_sta;
    unsigned char sec_sta;
};

extern struct keys key[4];
extern unsigned int frq;


#endif
