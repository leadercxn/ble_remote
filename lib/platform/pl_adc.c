/* Copyright (c) 2019 SENSORO Co.,Ltd. All Rights Reserved.
 *
 */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "app_error.h"
#include "nrf_drv_saadc.h"
#include "pl_adc.h"
 
#define ADC_DEFAULE_RESOLUTION     NRF_SAADC_RESOLUTION_12BIT
static nrf_saadc_input_t pl_adc_input_get(uint8_t input)
{
    nrf_saadc_input_t adc_input;
    switch(input)
    {
        case 0:
            adc_input = NRF_SAADC_INPUT_AIN0;
            break;
        case 1:
            adc_input = NRF_SAADC_INPUT_AIN1;
            break;
        case 2:
            adc_input = NRF_SAADC_INPUT_AIN2;
        break;
        case 3:
            adc_input = NRF_SAADC_INPUT_AIN3;
            break;
        case 4:
            adc_input = NRF_SAADC_INPUT_AIN4;
            break;
        case 5:
            adc_input = NRF_SAADC_INPUT_AIN5;
            break;
        case 6:
            adc_input = NRF_SAADC_INPUT_AIN6;
            break;
        case 7:
            adc_input = NRF_SAADC_INPUT_AIN7;
            break;
        case 8:
             adc_input = NRF_SAADC_INPUT_VDD;
            break;
        case 0xFF:
            adc_input = NRF_SAADC_INPUT_DISABLED;
            break;
        default:
            adc_input = NRF_SAADC_INPUT_DISABLED;
        break;
    }
    return adc_input;
}

void pl_adc_init(void)
{
    ret_code_t err_code;
    nrf_drv_saadc_config_t adc_config = NRF_DRV_SAADC_DEFAULT_CONFIG;
    adc_config.resolution = ADC_DEFAULE_RESOLUTION;
     
    err_code = nrf_drv_saadc_init(&adc_config,NULL);
    APP_ERROR_CHECK(err_code);
   
}
void pl_adc_uninit(void)
{
    nrf_drv_saadc_uninit();
}
void pl_adc_channel_init(uint8_t channel,uint8_t adc_input)
{
    ret_code_t err_code;
    nrf_saadc_input_t input = pl_adc_input_get(adc_input);
    nrf_saadc_channel_config_t channel_config = NRF_DRV_SAADC_DEFAULT_CHANNEL_CONFIG_SE(input);
    channel_config.reference = NRF_SAADC_REFERENCE_INTERNAL;
    channel_config.gain = NRF_SAADC_GAIN1_6; 
    err_code = nrf_drv_saadc_channel_init(channel, &channel_config);
    APP_ERROR_CHECK(err_code);
}
void pl_adc_channel_uninit(uint8_t channel)
{
     nrf_drv_saadc_channel_uninit(channel);
}

uint16_t pl_adc_sample_get(uint8_t channel)
{
    nrf_saadc_value_t value = 0;
    nrfx_saadc_sample_convert(channel,&value);
    return value;
}
uint16_t pl_adc_sample_millivolt_get(uint8_t channel)
{
    uint16_t result;
    nrf_saadc_value_t value = 0;
    nrfx_saadc_sample_convert(channel,&value);
    result = ADC_IN_MILLI_VOLTS(value);
    return result;
}
