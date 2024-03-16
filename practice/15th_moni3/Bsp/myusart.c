#include "myusart.h"
#include "usart.h"

unsigned char rx_pointer;
unsigned char rxdat;
unsigned char rxdata[30];

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    rxdata[rx_pointer++]=rxdat;
    HAL_UART_Receive_IT(&huart1,&rxdat,1);
}

