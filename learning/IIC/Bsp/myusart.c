#include "myusart.h"

unsigned char rxdata[30];
unsigned char rxdat;
unsigned char rx_pointer;

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    rxdata[rx_pointer++]=rxdat;
    HAL_UART_Receive_IT(&huart1,&rxdat,1);
}
