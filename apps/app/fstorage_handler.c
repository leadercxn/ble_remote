/**
 **********************************************************************************
  * @file    bsp_fstorage.c
  * @brief   nRF52832的flash操作驱动程序
  **********************************************************************************
  */
  
/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <string.h>
#include "nrf_fstorage.h"
#include "nrf_fstorage_sd.h"
#include "fds.h"
#include "nrf_log.h"

#include "fstorage_cfg.h"
#include "fstorage_handler.h"
#include "faccfg.pb.h"
#include "pb_decode.h"

#include "crc16.h"

storage_user_param_t    g_user_param;                     //user定义的数据结构
uint16_t                g_adv_fnt = 0 ;                   //记录遥控器按下的次数
uint16_t                g_adv_flash_offset = 0 ;          //fnt在对应储存的flash的地址偏置

const char ibuuid[16] = { 0x70, 0xDC, 0x44, 0xC3, 0xE2, 0xA8, 0x4B, 0x22, 0xA2, 0xC6, 0x12, 0x9B, 0x41, 0xA4, 0xBD, 0xBC };

static void fstorage_evt_handler(nrf_fstorage_evt_t * p_evt);

/**
 * start_addr 、end_addr限定flash可操作的范围
 */
NRF_FSTORAGE_DEF(nrf_fstorage_t fstorage) =
{
    .evt_handler = fstorage_evt_handler,
    .start_addr = FSTORAGE_START_ADDR,                 
    .end_addr   = FSTORAGE_END_ADDR,
};


static void fstorage_evt_handler(nrf_fstorage_evt_t * p_evt)
{
    if (p_evt->result != NRF_SUCCESS)
    {
        NRF_LOG_INFO("--> Event received: ERROR while executing an fstorage operation.");
        return;
    }

    switch (p_evt->id)
    {
        case NRF_FSTORAGE_EVT_WRITE_RESULT:
        {
            NRF_LOG_INFO("--> Event received: wrote %d bytes at address 0x%x.",
                         p_evt->len, p_evt->addr);
            NRF_LOG_HEXDUMP_INFO(p_evt->p_src, p_evt->len );     //打印数据
        } break;

        case NRF_FSTORAGE_EVT_ERASE_RESULT:
        {
            NRF_LOG_INFO("--> Event received: erased %d page from address 0x%x.",
                         p_evt->len, p_evt->addr);
        } break;

        default:
            break;
    }
}

#if 0
static uint32_t nrf5_flash_end_addr_get()
{
    uint32_t const bootloader_addr = NRF_UICR->NRFFW[0];
    uint32_t const page_sz         = NRF_FICR->CODEPAGESIZE;
    uint32_t const code_sz         = NRF_FICR->CODESIZE;

    return (bootloader_addr != 0xFFFFFFFF ?
            bootloader_addr : (code_sz * page_sz));
}

/**
 * @brief 等待flash的准备
 *       [不建议使用等待，因为擦除要等很久]
 */
void wait_for_flash_ready(void)
{
    /* While fstorage is busy, sleep and wait for an event. */
    while (nrf_fstorage_is_busy(&fstorage))
    {
       sd_app_evt_wait();
    }
}
#endif

/**
 * @brief fstorage初始化
 */
void fstorage_init(void)
{
    nrf_fstorage_api_t * p_fs_api;
    p_fs_api = &nrf_fstorage_sd;
    ret_code_t rc = nrf_fstorage_init(&fstorage, p_fs_api, NULL);
    APP_ERROR_CHECK(rc);
}


/**
 *@brief  fstorage 写
 *        注意： write_addr-> 写flash的地址应为4整数倍
 *              p_data-> 数据起始地址应为4整数倍   len->数据长度为4字节倍数
 */
void fstorage_write(uint32_t write_addr, void const * p_data, uint32_t len)
{
    len = ALIGN_NUM(4, len);    //字节对齐

    ret_code_t rc = nrf_fstorage_write(&fstorage, write_addr, p_data, len, NULL);
    APP_ERROR_CHECK(rc);
}


/**
 *@brief  fstorage 读
 */
void fstorage_read(uint32_t read_addr, void * p_data, uint32_t len)
{
    ret_code_t rc = nrf_fstorage_read(&fstorage, read_addr, p_data, len);
    APP_ERROR_CHECK(rc);
}

/**
 * @brief 擦出页
 */
void fstorage_erase(uint32_t page, uint32_t len)
{
	ret_code_t rc = nrf_fstorage_erase(&fstorage, page, len, NULL);
	APP_ERROR_CHECK(rc);
}





/**
 * @brief g_adv_fnt的保存
 */
void adv_fnt_store(void)
{
    static uint16_t adv_fnt_temp ;

    adv_fnt_temp = g_adv_fnt;

    if( g_adv_flash_offset > 1023 )             //写爆了4K的flash区，先擦除
    {
        g_adv_flash_offset = 0 ;
        fstorage_erase( ADV_FNT_ADDR, 1 );
    }
    fstorage_write( (ADV_FNT_ADDR + 4 * g_adv_flash_offset ), &adv_fnt_temp , sizeof(uint16_t) );

    NRF_LOG_INFO("store g_adv_fnt = %d ,g_adv_flash_offset = %d  at 0x%08x" , g_adv_fnt , g_adv_flash_offset , (ADV_FNT_ADDR + 4 * g_adv_flash_offset ) );

    g_adv_flash_offset++;
}


/**
 * @brief g_adv_fnt的读取
 */
uint16_t adv_fnt_get(void)
{
    uint32_t *p_data = (uint32_t *)( ADV_FNT_ADDR + (4 * (g_adv_flash_offset - 1)) ) ;
    uint16_t temp = (uint16_t) *p_data;
    return temp ;
}

/**
 * @brief  g_user_param保存
 */
void user_param_store_iflash(void)
{
    fstorage_erase( CONFIG_BACKUP_ADDR, 1 );            //先擦除flash
    fstorage_write( CONFIG_BACKUP_ADDR, &g_user_param , sizeof(storage_user_param_t) );
}

/**
 * @brief  g_user_param从出厂区读取
 */
void user_param_get_from_user_oflash(void)
{
    uint16_t app_param_crc = 0;

    storage_user_param_t *p_app_data = (storage_user_param_t *)CONFIG_BACKUP_ADDR;
    memcpy(&g_user_param, (uint8_t *)p_app_data, sizeof(storage_user_param_t));

    app_param_crc = crc16_compute((uint8_t const *)&p_app_data->crc + 2, sizeof(storage_user_param_t) - 4, NULL);

    NRF_LOG_INFO("storage_user_param_t size is : %d " , sizeof(storage_user_param_t) );

    NRF_LOG_INFO("user_oflash SN: " );
    NRF_LOG_HEXDUMP_INFO( p_app_data->sn, 8);     

    NRF_LOG_INFO("user_oflash appSecureKey:" );
    NRF_LOG_HEXDUMP_INFO(p_app_data->appSecureKey, 16); 

    NRF_LOG_INFO("user_oflash appBleTxPower:" );
    NRF_LOG_INFO("%d" , p_app_data->ble_txp );

    NRF_LOG_INFO("user_oflash appBleInterval:" );
    NRF_LOG_INFO("%d" , p_app_data->ble_int );

    NRF_LOG_INFO("user_oflash appBleFnt:" );
    NRF_LOG_INFO("%d" , p_app_data->blefnt);

    NRF_LOG_INFO("user_oflash appLowPowerLevel:" );
    NRF_LOG_INFO("%d" , p_app_data->lowpowerlevel);

    NRF_LOG_INFO("user_oflash crc:" );
    NRF_LOG_INFO("%d" , p_app_data->crc);

    NRF_LOG_INFO("app_param_crc:" );
    NRF_LOG_INFO("%d" , app_param_crc );

}




/**
 * @brief  g_user_param从出厂区读取
 */
uint8_t user_param_get_from_fac_oflash(void)
{
    bool        status;
    uint8_t     *p_config_data  = (uint8_t *) CONFIG_DATA_ADDR;
    uint16_t    config_data_len = 0;

    // 获取长度
    config_data_len = uint16_decode(p_config_data);
    NRF_LOG_INFO("config_data_len = %d", config_data_len);
    p_config_data = p_config_data + 2;

    //解析
    fac_Config config_param = fac_Config_init_zero;
    pb_istream_t stream = pb_istream_from_buffer(p_config_data, config_data_len);
    status = pb_decode(&stream, fac_Config_fields, &config_param);
    if(!status)
    {
        NRF_LOG_INFO("pb_decode false ");
        return NRF_ERROR_INTERNAL;
    }
    
    g_user_param.hw_ver = uint16_big_decode(config_param.appHardwareversion.bytes);
    memcpy(g_user_param.sn, config_param.appSn.bytes, config_param.appSn.size);     //把sn拷出来
    memcpy(g_user_param.appSecureKey, config_param.appSecureKey.bytes, config_param.appSecureKey.size);     //把appSecureKey拷出来
    g_user_param.blefnt = config_param.appBleFnt ;
    g_user_param.ble_txp = config_param.appBleTxPower ;
    g_user_param.ble_int = config_param.appBleInterval ;
    g_user_param.lowpowerlevel = config_param.appLowPowerLevel ;
    
    memcpy(g_user_param.ibeacon_uuid, ibuuid , sizeof(ibuuid) );     //把uuid拷出来
    g_user_param.ibeacon_major = uint16_big_decode(&g_user_param.sn[4]);
    g_user_param.ibeacon_minor = uint16_big_decode(&g_user_param.sn[6]);
    g_user_param.ibeacon_mrssi = -59;

    NRF_LOG_INFO("SN: " );
    NRF_LOG_HEXDUMP_INFO( &config_param.appSn.bytes , config_param.appSn.size);     

    NRF_LOG_INFO("appSecureKey:" );
    NRF_LOG_HEXDUMP_INFO(&config_param.appSecureKey.bytes, config_param.appSecureKey.size); 

    NRF_LOG_INFO("appBleTxPower:" );
    NRF_LOG_INFO("%d" , config_param.appBleTxPower );

    NRF_LOG_INFO("appBleInterval:" );
    NRF_LOG_INFO("%d" , config_param.appBleInterval );

    NRF_LOG_INFO("appBleFnt:" );
    NRF_LOG_INFO("%d" , config_param.appBleFnt);

    NRF_LOG_INFO("appLowPowerLevel:" );
    NRF_LOG_INFO("%d" , config_param.appLowPowerLevel);

    NRF_LOG_INFO("ibeacon_uuid:" );
    NRF_LOG_HEXDUMP_INFO(g_user_param.ibeacon_uuid, sizeof(g_user_param.ibeacon_uuid) ); 

    NRF_LOG_INFO("ibeacon_major:" );
    NRF_LOG_INFO("0x%04x" , g_user_param.ibeacon_major);

    NRF_LOG_INFO("ibeacon_minor:" );
    NRF_LOG_INFO("0x%04x" , g_user_param.ibeacon_minor);

    NRF_LOG_INFO("hw_version:" );
    NRF_LOG_INFO("0x%04x" , g_user_param.hw_ver);

    
    return NRF_SUCCESS;
}




/******************* (C) COPYRIGHT 2018 HTGD *****END OF FILE****/




