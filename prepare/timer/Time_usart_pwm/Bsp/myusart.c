#include "myusart.h"
#include "usart.h"

char rxdata[30];//����������ݣ��Ӵ���������������ݣ�
unsigned char rxpointer;//��ʾ�����˶��ٸ��ַ�
unsigned char rxdat;//ÿ�ν��յ��ַ�


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    rxdata[rxpointer++]=rxdat;
    HAL_UART_Receive_IT(&huart1,&rxdat,1);
}
