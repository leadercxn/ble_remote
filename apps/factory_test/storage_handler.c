/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <string.h>
#include "nrf_fstorage.h"
#include "nrf_fstorage_nvmc.h"          //nvmc的方式对flash操作
#include "fds.h"
#include "nrf_log.h"
#include "crc16.h"

#include "storage_handler.h"

static void fstorage_evt_handler(nrf_fstorage_evt_t * p_evt);

/**
 * start_addr 、end_addr限定flash可操作的范围
 */
NRF_FSTORAGE_DEF(nrf_fstorage_t fstorage) =
{
    .evt_handler = fstorage_evt_handler,
    .start_addr = STORAGE_START_ADDR,                 
    .end_addr   = STORAGE_END_ADDR,
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
    p_fs_api = &nrf_fstorage_nvmc;
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



























