#ifndef __AES_HANDLER_H
#define __AES_HANDLER_H

extern nrf_ecb_hal_data_t  cc_ecb_data ;

/**
 * @brief 硬件随机数初始化
 */
void rng_init(void);

/**
 * @brief 获取产生的随机数m_rng
 */
uint8_t rng_get(void);

/**
 * @brief cleartext 与 key 加密
 */
void aes_encrypt(void);

#endif




