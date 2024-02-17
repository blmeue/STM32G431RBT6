#include "seg.h"

#define SEG_H HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,GPIO_PIN_SET);
#define SEG_L HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,GPIO_PIN_RESET);

//RCK存储寄存器的时钟引脚
#define RCK_H HAL_GPIO_WritePin(GPIOA,GPIO_PIN_2,GPIO_PIN_SET);
#define RCK_L HAL_GPIO_WritePin(GPIOA,GPIO_PIN_2,GPIO_PIN_RESET);

//SCK移位寄存器的时钟引脚
#define SCK_H HAL_GPIO_WritePin(GPIOA,GPIO_PIN_3,GPIO_PIN_SET);
#define SCK_L HAL_GPIO_WritePin(GPIOA,GPIO_PIN_3,GPIO_PIN_RESET);

unsigned char seg_code[]={                       //标准字库
//   0    1    2    3    4    5    6    7    8    9    A    B    C    D    E    F
    0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F,0x77,0x7C,0x39,0x5E,0x79,0x71,
  //black  -     H    J    K    L    N    o   P    U     t    G    Q    r   M    y
    0x00,0x40,0x76,0x1E,0x70,0x38,0x37,0x5C,0x73,0x3E,0x78,0x3d,0x67,0x50,0x37,0x6e,
    0xBF,0x86,0xDB,0xCF,0xE6,0xED,0xFD,0x87,0xFF,0xEF,0x46};    //0. 1. 2. 3. 4. 5. 6. 7. 8. 9. -1

//数码管显示缓存
unsigned char seg_buf[3];

//注意：先发送最高位，即第三个数码管的最高位数据    
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
            //移位寄存器处于上升沿，SRCLK每来一个上升沿，就往移位寄存器里送一位数据
            SCK_L;
            SCK_H;
            //左移1位
            seg_buf[j-1]<<=1;
        }
    }
    
    //RCLK上升沿，移位寄存器的数据送入存储寄存器
    RCK_L;
    RCK_H;
    
}
