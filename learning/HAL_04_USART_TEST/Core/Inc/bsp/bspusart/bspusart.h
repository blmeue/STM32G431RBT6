#ifndef __BSPUSART_H__
#define __BSPUSART_H__

#include "main.h"

extern UART_HandleTypeDef huart1;
extern char rxdata[30];//���մ���õ�����
extern uint8_t rxdat;//ÿ�ν��յ�����
extern unsigned char rx_pointer;//ȷ��һ��д���ĸ�λ����
void USART1_Init(void);

#endif
