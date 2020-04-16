/* Copyright (c) 2019 SENSORO Co.,Ltd. All Rights Reserved.
 *
 */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include "pl_adc.h"
#include "battery_handler.h"

#define BATTERY_ADC_INPUT    PL_ADC_INPUT_VDD 
#define BATTERY_ADC_CHANNEL  1

#define FILTER_BUFF_LEN 6


#if 0
static int cmpfunc(const void * a, const void * b)
{
    return (*(uint16_t *) a - * (uint16_t *) b);
}


static uint16_t battery_mvolt_filter(uint16_t mvolt)
{
    static uint16_t filter_buff[FILTER_BUFF_LEN] = {0};
    static uint16_t sort_buff[FILTER_BUFF_LEN]   = {0};
    static uint8_t  filter_cnt                   = 0;
    static uint8_t  filter_index                 = 0;

    uint32_t mvolt_ret = 0;

    filter_buff[filter_index++] = mvolt;
    filter_cnt++;

    filter_index = filter_index % FILTER_BUFF_LEN;

    if(filter_cnt > FILTER_BUFF_LEN)
    {
        filter_cnt = FILTER_BUFF_LEN;
    }

    // ??????5, ?????
    if(filter_cnt < 5)
    {
        for(uint8_t i = 0; i < filter_cnt; i++)
        {
            mvolt_ret += filter_buff[i];
        }
        return mvolt_ret / filter_cnt;
    }
    // ??????5, ????????????
    else
    {
        for(uint8_t i = 0; i < filter_cnt; i++)
        {
            sort_buff[i] = filter_buff[i];
        }

        // ??
        qsort(sort_buff, filter_cnt, sizeof(uint16_t), cmpfunc);

        for(uint8_t i = 1; i < filter_cnt - 1; i++)
        {
            mvolt_ret += sort_buff[i];
        }
        return mvolt_ret / (filter_cnt - 2);
    }
}
#endif

uint16_t battery_sample(void)
{
    return pl_adc_sample_millivolt_get(BATTERY_ADC_CHANNEL);
}

void battery_init(void)
{
    pl_adc_init();
    pl_adc_channel_init(BATTERY_ADC_CHANNEL,BATTERY_ADC_INPUT);
}

void battery_uninit(void)
{
    pl_adc_channel_uninit(BATTERY_ADC_CHANNEL);
    pl_adc_uninit();
}
