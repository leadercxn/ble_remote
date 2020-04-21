/* Copyright (c) 2020 SENSORO Co.,Ltd. All Rights Reserved.
 *
 */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "app_error.h"
#include "app_util.h"
#include "nrf_drv_rng.h"
#include "nrf_soc.h"
#include "nrf_delay.h"

#include "nrf_log.h"

#include "remote_cmd.h"

#define REMOTE_CMD_DATA_COUNT_OFFSET    5
#define REMOTE_CMD_DATA_NONCE_OFFSET    7
#define REMOTE_CMD_DATA_CMD_OFFSET      8
#define REMOTE_CMD_DATA_CIPHER_OFFSET   11

static uint8_t m_key[16] = {0};

static void aes_ecb_encrypt(const uint8_t in[16], uint8_t out[16], const uint8_t key[16])
{
    nrf_ecb_hal_data_t m_aes_ecb;

    memcpy(m_aes_ecb.key, key, 16);
    memcpy(m_aes_ecb.cleartext, in, 16);
    sd_ecb_block_encrypt(&m_aes_ecb);
    memcpy(out, m_aes_ecb.ciphertext, 16);
}

static void random_vector_generate(uint8_t * p_buff, uint8_t size)
{
    uint32_t err_code;
    uint8_t  available = 0;

    if(size == 0)
    {
        return;
    }

    nrf_drv_rng_bytes_available(&available);

    while(available < size)
    {
        nrf_delay_ms(1);
        nrf_drv_rng_bytes_available(&available);
    }

    err_code = nrf_drv_rng_rand(p_buff, size);
    APP_ERROR_CHECK(err_code);
}

void remote_cmd_data_get(uint8_t cmd, uint16_t count, uint8_t * p_cmd_data, uint16_t * p_size)
{
    uint8_t nonce = 0;
    uint8_t plain[16] = {0};
    uint8_t salt[16] = {0};

    uint8_t data[] =
    {
        0x1A, 0x16, 0xDB, 0x77,                                 // 8B 包头
        0x10,                                                   // 软件版本号 , 用于决定后面的数据格式
        0x00, 0x00,                                             // count
        0x00,                                                   // Nonce
        0x00,                                                   // 按键键值
        0x00, 0x00,                                             // SN(2B)
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };

    random_vector_generate(&nonce, 1);

    NRF_LOG_INFO("cmd = %02X, count = %d, nonce = %02X", cmd, count, nonce);

    plain[0] = cmd;

    NRF_LOG_INFO("plain =");
    NRF_LOG_HEXDUMP_INFO(plain, 16);

    uint16_encode(count, salt);
    salt[2] = nonce;

    NRF_LOG_INFO("plain salt =");
    NRF_LOG_HEXDUMP_INFO(salt, 16);

    aes_ecb_encrypt(salt, salt, m_key);

    NRF_LOG_INFO("cipher salt =");
    NRF_LOG_HEXDUMP_INFO(salt, 16);

    uint16_encode(count, data + REMOTE_CMD_DATA_COUNT_OFFSET);
    data[REMOTE_CMD_DATA_NONCE_OFFSET] = nonce;
    data[REMOTE_CMD_DATA_CMD_OFFSET] = cmd;

    for(uint32_t i = 0; i < 16; i++)
    {
        data[REMOTE_CMD_DATA_CIPHER_OFFSET + i] = plain[i] ^ salt[i];
    }

    memcpy(p_cmd_data, data, sizeof(data));
    *p_size = sizeof(data);

    NRF_LOG_INFO("result =");
    NRF_LOG_HEXDUMP_INFO(p_cmd_data, *p_size);
}

void remote_cmd_init(void)
{
    uint32_t err_code;
    err_code = nrf_drv_rng_init(NULL);
    APP_ERROR_CHECK(err_code);
}

void remote_cmd_key_set(uint8_t key[16])
{
    memcpy(m_key, key, 16);
}
