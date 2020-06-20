#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "ble.h"
#include "app_error.h"
#include "app_scheduler.h"
#include "app_timer.h"
#include "ble_hci.h"

#include "ble_pwd.h"
#include "ble_pwd_handler.h"

ble_pwd_t m_pwd;



/**
 * 读操作
 */
static void  read_handler( ble_pwd_t *p_pwd , ble_pwd_evt_type_e evt_type, uint16_t value_handle )
{
#if 0
    if( BLE_CONN_HANDLE_INVALID != p_pwd->conn_handle )
    {
        uint8_t buff[10] = { 0x00 ,0x01 ,0x02 ,0x03 ,0x04 ,0x05 ,0x06 ,0x07 ,0x08 ,0x09 };
        uint16_t buff_len = sizeof(buff);
        ble_gatts_hvx_params_t hvx_params;
        memset(&hvx_params, 0, sizeof(hvx_params));

        hvx_params.handle   = p_pwd->pwd_char_handle.value_handle  ;//char_handle.value_handle;
        hvx_params.type     = BLE_GATT_HVX_INDICATION;
        hvx_params.offset   = 0;
        hvx_params.p_len    = &buff_len;
        hvx_params.p_data   = buff;

        sd_ble_gatts_hvx(p_pwd->conn_handle, &hvx_params);      //  发送数据
    }
#endif
}


/**
 *@brief 密码服务初始化 
 */
void ble_srv_pwd_init(void)
{
    uint32_t        err_code = 0 ;
    ble_pwd_init_t  init ;
    uint8_t buff[5]={0x01 ,0x02 ,0x03,0x04, 0x05};

    init.read_handler = read_handler ;
    init.version = buff ;

    err_code = ble_pwd_init( &m_pwd , &init );
    APP_ERROR_CHECK(err_code);
}

/**
 *@brief pwd的事件派发
 */
void on_ble_srv_pwd_evt(ble_evt_t* p_ble_evt)
{
    ble_pwd_on_ble_evt( &m_pwd ,  p_ble_evt ) ;
}



