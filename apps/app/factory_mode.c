#include <stdint.h>
#include <string.h>
#include "nordic_common.h"
#include "nrf.h"
#include "app_error.h"
#include "ble.h"
#include "ble_err.h"
#include "ble_hci.h"
#include "ble_srv_common.h"
#include "ble_advdata.h"
#include "ble_conn_params.h"
#include "nrf_sdh.h"
#include "nrf_sdh_ble.h"
#include "boards.h"
#include "app_timer.h"
#include "app_button.h"
#include "nrf_ble_gatt.h"
#include "app_scheduler.h"
#include "nrf_ble_qwr.h"
#include "nrf_pwr_mgmt.h"
#include "app_util.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#include "timer.h"
#include "normal_mode.h"
#include "data_handler.h"

#include "fstorage_cfg.h"
#include "fstorage_handler.h"
#include "factory_mode.h"
#include "faccfg.pb.h"
#include "pb_decode.h"
#include "version.h"

#define HW_VERSION    0x0101
#define CHIP_ID_ADDR  0x10000060

static storage_user_param_t    xuser_param;            //写入flash区的,用全局变量定义,user定义的数据结构



#if 0
/**
 * @brief   获取芯片的ID
 * @param[out]   data   存放chip_id的缓存
 */
static void chip_id_get(uint8_t *data)
{
    memcpy(data, (uint8_t *)CHIP_ID_ADDR, 8);
}
#endif

/**
 *@brief  读取 flash 中的配置数据进行配置
 */
bool config_from_flash(void)
{
    uint32_t            *p_data         = (uint32_t *) CONFIG_DATA_ADDR;
    uint32_t            *p              = (uint32_t *) HW_VERSION_ADDR;
    bool                dis_warming     = true ;
    
    if( 0xFFFFFFFF == *p_data )                     //数据还没配置
    {
        if( true == dis_warming )
        {
            NRF_LOG_INFO("No config_data in flash " );
            dis_warming = false ;
        }
        
        if( 0xFFFFFFFF == *p )                      //HW_VERSION地址还没数据
        {
            xuser_param.magic_byte = FSTORAGE_MAGIC_BYTE ;
            //xuser_param.hw_ver = 0x0102 ;
            //xuser_param.fw_ver = 0x1200 ;
            xuser_param.hw_ver = HW_VERSION ;
            xuser_param.fw_ver = APP_VERSION ;
            fstorage_write( HW_VERSION_ADDR, &xuser_param, (sizeof(xuser_param.hw_ver)+sizeof(xuser_param.fw_ver)) );   //只写入硬件版本和软件版本号，用于脚本拉去配置数据
            NRF_LOG_INFO("write hw_ver && fw_ver into flash " );
        }
        return false ;
    }
    return true ;
}

/**
 * @brief 出厂模式
 */
void factory_mode(void)
{
    NRF_LOG_INFO("loop in factory mode " );
    for (;;)
    {
        app_sched_execute();
        idle_state_handle();
    }
}







