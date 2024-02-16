#ifndef __INTERRUPT_H__
#define __INTERRUPT_H__

#include "main.h"
#include "stdbool.h"

struct keys{
  uchar judge_sta;//判断进行到哪一步
  bool key_sta;//识别到硬件按键被按下为0
  bool single_flag;//确认被按下时为1
  bool long_flag;//长按键标志

  uint key_time;//按下时间
    
};

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);

#endif
