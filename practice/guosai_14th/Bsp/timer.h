#ifndef __TIMER_H__
#define __TIMER_H__

#include "main.h"
#include "stdbool.h"

struct keys{
    unsigned char judge_sta;
    bool key_sta;
    bool single_flag;
    bool long_flag;
    unsigned char sec_sta;
};

extern struct keys key[4];
extern double duty;
extern unsigned int frq;

void key_scan(void);


#endif
