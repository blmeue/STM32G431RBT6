#include "myusart.h"

unsigned char rxdata[30];
unsigned char rxdat;
unsigned char rxpointer;

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    rxdata[rxpointer++]=rxdat;
    HAL_UART_Receive_IT(&huart1,&rxdat,1);
}
