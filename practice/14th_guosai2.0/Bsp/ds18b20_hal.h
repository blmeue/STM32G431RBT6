#ifndef __DS18B20_HAL_H
#define __DS18B20_HAL_H

#include "stm32g4xx_hal.h"

#define OW_PIN_PORT		    GPIOA
#define OW_PIN				GPIO_PIN_6


#define OW_DIR_OUT() 	    mode_output1()
#define OW_DIR_IN() 	    mode_input1()
#define OW_OUT_LOW() 	    (HAL_GPIO_WritePin(OW_PIN_PORT, OW_PIN, GPIO_PIN_RESET))
#define OW_GET_IN()  	    (HAL_GPIO_ReadPin(OW_PIN_PORT, OW_PIN))
#define OW_SKIP_ROM 		0xCC
#define DS18B20_CONVERT 	0x44
#define DS18B20_READ 		0xBE


void ds18b20_init_x(void);
//s16 ds18b20_read(void);
double temperature_read(void);



#endif

