#ifndef __BSPUSART_H__
#define __BSPUSART_H__

#include "main.h"

extern UART_HandleTypeDef huart1;
extern char rxdata[30];//最终处理好的数据
extern uint8_t rxdat;//每次接收的数据
extern unsigned char rx_pointer;//确定一下写到哪个位置了
void USART1_Init(void);

#endif
