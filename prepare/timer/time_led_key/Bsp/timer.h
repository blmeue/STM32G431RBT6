#ifndef __TIMER_H__
#define __TIMER_H__

#include "main.h"
#include "stdbool.h"

struct keys
{
    unsigned char judge_sta;//�жϽ��е��Ĳ���
    bool key_sta;//ʶ��Ӳ������Ϊ0
    bool single_flag;//ȷ�ϱ�����Ϊ1�����±�־λ
};

extern struct keys key[4];
#endif
