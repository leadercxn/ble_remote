/* Copyright (c) 2019 SENSORO Co.,Ltd. All Rights Reserved.
 *
 */

#ifndef PL_UTILS_H__
#define PL_UTILS_H__
#include "nrf_delay.h"
#define pl_delay_ms  nrf_delay_ms

void pl_pool_init(void);
void *pl_getbuf (uint16_t size);
void pl_freebuf (void *bptr);
void pl_ringbuf_init(uint8_t idx);
uint32_t pl_ringbuf_put(uint8_t idx,uint8_t *pbuf,uint16_t len);
uint32_t pl_ringbuf_get(uint8_t idx,uint8_t *pbuf,uint16_t len);
#endif
