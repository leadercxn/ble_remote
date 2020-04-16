/* Copyright (c) 2019 SENSORO Co.,Ltd. All Rights Reserved.
 *
 */

#ifndef __PL_ADC_H
#define __PL_ADC_H

#define ADC_GAIN            6  
#define ADC_MAX_VOLTAGE_IN_MILLIVOLTS   (600 * ADC_GAIN)  
#define ADC_IN_MILLI_VOLTS(ADC_VALUE)  \
          (((ADC_VALUE) * ADC_MAX_VOLTAGE_IN_MILLIVOLTS) /4096)


typedef enum
{
    PL_ADC_INPUT_0,
    PL_ADC_INPUT_1,
    PL_ADC_INPUT_2,
    PL_ADC_INPUT_3,
    PL_ADC_INPUT_4,
    PL_ADC_INPUT_5,
    PL_ADC_INPUT_6,
    PL_ADC_INPUT_7,
    PL_ADC_INPUT_VDD,
    PL_ADC_INPUT_DISABLED
} pl_adc_input_t;

void pl_adc_init(void);
void pl_adc_uninit(void);
void pl_adc_channel_init(uint8_t channel,uint8_t adc_input);
void pl_adc_channel_uninit(uint8_t channel);
uint16_t pl_adc_sample_get(uint8_t channel);
uint16_t pl_adc_sample_millivolt_get(uint8_t channel);
#endif
