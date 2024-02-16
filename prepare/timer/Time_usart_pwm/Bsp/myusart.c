#include "myusart.h"
#include "usart.h"

char rxdata[30];//保存接收数据（从串口助手输入的数据）
unsigned char rxpointer;//表示接收了多少个字符
unsigned char rxdat;//每次接收的字符


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    rxdata[rxpointer++]=rxdat;
    HAL_UART_Receive_IT(&huart1,&rxdat,1);
}
