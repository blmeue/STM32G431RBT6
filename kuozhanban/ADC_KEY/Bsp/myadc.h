#ifndef __MYADC_H__
#define __MYADC_H__

#include "main.h"
double get_adc(ADC_HandleTypeDef *pin);
unsigned int get_adcValue(ADC_HandleTypeDef *pin);

#endif
