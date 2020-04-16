#ifndef __FSTORAGE_HANDLER_H
#define	__FSTORAGE_HANDLER_H

#include "nrf_fstorage.h"
#if (FDS_BACKEND == NRF_FSTORAGE_SD)
#include "nrf_fstorage_sd.h"
#endif

#if (FDS_CRC_CHECK_ON_READ)
#include "crc16.h"
#endif

#define FSTORAGE_MAGIC_BYTE     0x08



/**
 * @brief 用户参数结构
 */
typedef struct 
{
    uint16_t    hw_ver;
    uint16_t    fw_ver;
    uint16_t    magic_byte;
    uint16_t    crc;
    uint8_t     sn[8];
    uint8_t     appSecureKey[16];

    uint8_t     ibeacon_uuid[16];
    uint16_t    ibeacon_major;
    uint16_t    ibeacon_minor;
    int8_t      ibeacon_mrssi;

    int8_t      ble_txp;                     
    uint16_t    ble_int;
    uint16_t    blefnt;
    uint16_t    lowpowerlevel;
} __attribute__((aligned(4))) storage_user_param_t;

extern storage_user_param_t    g_user_param;                 //user定义的数据结构
extern uint16_t                g_adv_fnt ;                   //记录遥控器按下的次数
extern uint16_t                g_adv_flash_offset ;          //fnt在对应储存的flash的地址偏置




/**
 * @brief fstorage初始化
 */
void fstorage_init(void);

/**
 *@brief  fstorage 写
 *        注意： write_addr-> 写flash的地址应为4整数倍
 *              p_data-> 数据起始地址应为4整数倍   len->数据长度为4字节倍数
 */
void fstorage_write(uint32_t write_addr, void const * p_data, uint32_t len);

/**
 *@brief  fstorage 读
 */
void fstorage_read(uint32_t read_addr, void * p_data, uint32_t len);

/**
 * @brief 擦出页
 */
void fstorage_erase(uint32_t page, uint32_t len);

/**
 * @brief g_adv_fnt的保存
 */
void adv_fnt_store(void);

/**
 * @brief g_adv_fnt的读取
 */
uint16_t adv_fnt_get(void);

/**
 * @brief  g_user_param保存
 */
void user_param_store_iflash(void);

/**
 * @brief  g_user_param从出厂区读取
 */
void user_param_get_from_user_oflash(void);

/**
 * @brief  g_user_param从出厂区读取
 */
uint8_t user_param_get_from_fac_oflash(void);

#endif	/* __BSP_FSTORAGE_H */


