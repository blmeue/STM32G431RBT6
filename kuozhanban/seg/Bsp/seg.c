#include "seg.h"

#define SEG_H HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,GPIO_PIN_SET);
#define SEG_L HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,GPIO_PIN_RESET);

//RCK�洢�Ĵ�����ʱ������
#define RCK_H HAL_GPIO_WritePin(GPIOA,GPIO_PIN_2,GPIO_PIN_SET);
#define RCK_L HAL_GPIO_WritePin(GPIOA,GPIO_PIN_2,GPIO_PIN_RESET);

//SCK��λ�Ĵ�����ʱ������
#define SCK_H HAL_GPIO_WritePin(GPIOA,GPIO_PIN_3,GPIO_PIN_SET);
#define SCK_L HAL_GPIO_WritePin(GPIOA,GPIO_PIN_3,GPIO_PIN_RESET);

unsigned char seg_code[]={                       //��׼�ֿ�
//   0    1    2    3    4    5    6    7    8    9    A    B    C    D    E    F
    0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F,0x77,0x7C,0x39,0x5E,0x79,0x71,
  //black  -     H    J    K    L    N    o   P    U     t    G    Q    r   M    y
    0x00,0x40,0x76,0x1E,0x70,0x38,0x37,0x5C,0x73,0x3E,0x78,0x3d,0x67,0x50,0x37,0x6e,
    0xBF,0x86,0xDB,0xCF,0xE6,0xED,0xFD,0x87,0xFF,0xEF,0x46};    //0. 1. 2. 3. 4. 5. 6. 7. 8. 9. -1

//�������ʾ����
unsigned char seg_buf[3];

//ע�⣺�ȷ������λ��������������ܵ����λ����    
void seg_display(void)
{
    unsigned char i;
    unsigned int j;
    for(j=3;j>0;j--){
        for(i=0;i<8;i++)
        {
            if(seg_buf[j-1]&0x80)
            {
                SEG_H;
            }
            else
                SEG_L;
            //��λ�Ĵ������������أ�SRCLKÿ��һ�������أ�������λ�Ĵ�������һλ����
            SCK_L;
            SCK_H;
            //����1λ
            seg_buf[j-1]<<=1;
        }
    }
    
    //RCLK�����أ���λ�Ĵ�������������洢�Ĵ���
    RCK_L;
    RCK_H;
    
}
