#ifndef __TIMER_H__
#define __TIMER_H__

#include "main.h"
#include "stdbool.h"

struct keys
{
    unsigned char judge_sta;//判断进行到哪步了
    bool key_sta;//识别到硬件按下为0
    bool single_flag;//确认被按下为1，按下标志位
};

extern struct keys key[4];
#endif
