#ifndef __INTERRUPT_H__
#define __INTERRUPT_H__

#include "main.h"
#include "stdbool.h"

struct keys{
  uchar judge_sta;//�жϽ��е���һ��
  bool key_sta;//ʶ��Ӳ������������Ϊ0
  bool single_flag;//ȷ�ϱ�����ʱΪ1
  bool long_flag;//��������־

  uint key_time;//����ʱ��
    
};

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);

#endif
