#ifndef __INTERRUPT_H__
#define __INTERRUPT_H__

#include "main.h"
#include "stdbool.h"

struct keys{
  uchar judge_sta;//判断进行到哪一步
  bool key_sta;//识别到硬件按键被按下为0
  bool single_flag;//确认被按下时为1
};

#endif
