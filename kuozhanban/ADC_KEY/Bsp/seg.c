#include "seg.h"

#define SER_H HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,GPIO_PIN_SET)
#define SER_L HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,GPIO_PIN_RESET)

//RCK存储寄存器的时钟引脚
#define RCK_H HAL_GPIO_WritePin(GPIOA,GPIO_PIN_2,GPIO_PIN_SET)
#define RCK_L HAL_GPIO_WritePin(GPIOA,GPIO_PIN_2,GPIO_PIN_RESET)

//SCK移位寄存器的时钟引脚
#define SCK_H HAL_GPIO_WritePin(GPIOA,GPIO_PIN_3,GPIO_PIN_SET)
#define SCK_L HAL_GPIO_WritePin(GPIOA,GPIO_PIN_3,GPIO_PIN_RESET)

unsigned char seg_code[]={                       //标准字库
//   0    1    2    3    4    5    6    7    8    9    A    B    C    D    E    F
    0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F,0x77,0x7C,0x39,0x5E,0x79,0x71,
  //black  -    H    L    n    o   P    U     G   q   R
    0x00,0x40,0x76,0x38,0x37,0x5C,0x73,0x3E,0x3d,0x67,0x77,
    //0.  1.    2.   3.  4.   5.    6.   7.   8.  9.   -1
    0xBF,0x86,0xDB,0xCF,0xE6,0xED,0xFD,0x87,0xFF,0xEF,0x46
};
unsigned char seg_buf[3];

void seg_disp(void)
{
    int i,j;
    
    for(j=2;j>=0;j--)
    {
        for(i=0;i<8;i++)
        {
            if(seg_buf[j]&0x80)
                SER_H;
            else
                SER_L;
            //移位寄存器处于上升沿，SRCLK每来一个上升沿，就往移位寄存器里送一位数据
            SCK_L;
            SCK_H;
            
            seg_buf[j]<<=1;
        }
    }
    //RCLK上升沿，移位寄存器的数据送入存储寄存器
    RCK_L;
    RCK_H;
}
