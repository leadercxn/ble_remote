/* Copyright (c) 2019 SENSORO Co.,Ltd. All Rights Reserved.
 *
 */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "nrf_delay.h"
#include "nrf_ringbuf.h"
#include "mem_manager.h"
#include "pl_utils.h"
//code for ringbuf end
#define RING_BUF_SIZE  128
NRF_RINGBUF_DEF(ring1,RING_BUF_SIZE);//define a rinfbuf for atrx

void pl_ringbuf_init(uint8_t idx)
{
    nrf_ringbuf_init(&ring1);
}
uint32_t pl_ringbuf_put(uint8_t idx,uint8_t *pbuf,uint16_t len)
{
    uint16_t length = len;
    return nrf_ringbuf_cpy_put(&ring1,pbuf,(size_t *)&length);
}
uint32_t pl_ringbuf_get(uint8_t idx,uint8_t *pbuf,uint16_t len)
{
    uint16_t length = len;
    return nrf_ringbuf_cpy_get(&ring1,pbuf,(size_t *)&length);
}
//code for ringbuf end
//code for memory pool
void pl_pool_init(void)
{
    nrf_mem_init();
}
void *pl_getbuf (uint16_t size)
{
    return nrf_malloc(size);
}

void pl_freebuf (void *bptr)
{
    nrf_free(bptr);

}
//code for memory pool end

