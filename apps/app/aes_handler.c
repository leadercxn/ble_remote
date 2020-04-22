#include <stdint.h>
#include <string.h>
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "nrf_drv_rng.h"

#include "fstorage_handler.h"
#include "aes_handler.h"

nrf_ecb_hal_data_t  cc_ecb_data ;

static uint8_t m_salt[16] = { 0 };
static uint8_t m_rng = 0 ;

/**
 * @brief 硬件随机数初始化
 */
void rng_init(void)
{
    nrf_drv_rng_init(NULL);
}


/**
 * @brief  获取一个字节的随机数
 */
static uint8_t one_byte_rng_generate(void)
{
    nrf_drv_rng_rand( &m_rng , sizeof(uint8_t) );

    return m_rng ; 
}

/**
 * @brief 获取产生的随机数m_rng
 */
uint8_t rng_get(void)
{
    return m_rng;
}



/**
 * @brief salt padding
 */
static void salt_padding(void)
{
    uint16_encode( g_adv_fnt , &m_salt[0] ); 
    m_salt[2] = one_byte_rng_generate();

    NRF_LOG_DEBUG(" salt_padding : " );
    NRF_LOG_HEXDUMP_DEBUG(m_salt, sizeof(m_salt));     //打印数据
}


/**
 * @brief cleartext 与 key 加密
 */
void aes_encrypt(void)
{
    salt_padding();

    memset( cc_ecb_data.key , 0 , sizeof(cc_ecb_data.key) );                                //key
    memcpy( cc_ecb_data.key , g_user_param.appSecureKey , sizeof(cc_ecb_data.key)   );

    memset( cc_ecb_data.cleartext , 0 , sizeof(cc_ecb_data.cleartext) );                    //cleartext
    memcpy( cc_ecb_data.cleartext , m_salt , sizeof(cc_ecb_data.cleartext)   );


    sd_ecb_block_encrypt(&cc_ecb_data);

    NRF_LOG_DEBUG("encrypt.key: " );
    NRF_LOG_HEXDUMP_DEBUG(cc_ecb_data.key, sizeof(cc_ecb_data.key));                   //打印数据

    NRF_LOG_DEBUG("encrypt.cleartext: " );
    NRF_LOG_HEXDUMP_DEBUG(cc_ecb_data.cleartext, sizeof(cc_ecb_data.cleartext));       //打印数据

    NRF_LOG_DEBUG("encrypt.ciphertext: " );
    NRF_LOG_HEXDUMP_DEBUG(cc_ecb_data.ciphertext, sizeof(cc_ecb_data.ciphertext));     //打印数据

}

