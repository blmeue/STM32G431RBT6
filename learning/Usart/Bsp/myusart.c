#include "myusart.h"

char rxdata[30];//���մ���õ�����
uint8_t rxdat;//ÿ�ν��յ�����
unsigned char rx_pointer;//ȷ��һ��д���ĸ�λ����

//���ڽ��յĻص�����
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    rxdata[rx_pointer++]=rxdat;
    HAL_UART_Receive_IT(&huart1,&rxdat,1);
}

