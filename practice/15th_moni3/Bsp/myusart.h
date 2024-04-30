#ifndef __MYUSART_H__
#define __MYUSART_H__

#include "main.h"

extern unsigned char rx_pointer;
extern unsigned char rxdat;
extern unsigned char rxdata[30];

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);

#endif
