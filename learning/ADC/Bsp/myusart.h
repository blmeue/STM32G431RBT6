#ifndef __MYUSART_H__
#define __MYUSART_H__

#include "main.h"
#include "usart.h"

extern unsigned char rxdata[30];
extern unsigned char rxdat;
extern unsigned char rxpointer;

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);

#endif
