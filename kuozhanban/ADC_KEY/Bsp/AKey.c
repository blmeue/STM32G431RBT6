#include "AKey.h"
#include "myadc.h"

extern unsigned int adc_value;
    
unsigned char akey_scan(void)
{
    unsigned char key_num=0;
    
    if(adc_value<5)
        key_num=1;
    else if(adc_value>520&&adc_value<540)
        key_num=2;
    else if(adc_value>1100&&adc_value<1200)
        key_num=3;
    else if(adc_value>1700&&adc_value<1800)
        key_num=4;
    else if(adc_value>2300&&adc_value<2400)
        key_num=5;
    else if(adc_value>2700&&adc_value<2900)
        key_num=6;
    else if(adc_value>3400&&adc_value<3600)
        key_num=7;
    else if(adc_value>3900&&adc_value<4000)
        key_num=8;
    else 
        key_num=0;
    
    return key_num;
}
