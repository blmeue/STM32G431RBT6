#include"led.h"


void LED_Disp(uchar dsLED)
{
    //���������øߵ�ƽ��������LED����
    HAL_GPIO_WritePin(GPIOC,GPIO_PIN_All,GPIO_PIN_SET);
    //dsledĬ����ǰ��λ�����ư�λ���Ϳ��Կ���PC15-PC8
    //���� dsLED=0x65,��0110 0101
    //���ư�λ�ͱ�� 0110 0101 0000 0000
    HAL_GPIO_WritePin(GPIOC,dsLED<<8,GPIO_PIN_RESET);
    //����������
    HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_RESET);
    
}
