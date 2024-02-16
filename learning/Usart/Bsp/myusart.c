#include "myusart.h"

char rxdata[30];//最终处理好的数据
uint8_t rxdat;//每次接收的数据
unsigned char rx_pointer;//确定一下写到哪个位置了

//串口接收的回调函数
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    rxdata[rx_pointer++]=rxdat;
    HAL_UART_Receive_IT(&huart1,&rxdat,1);
}

