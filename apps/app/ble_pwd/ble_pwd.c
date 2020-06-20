/* Copyright (c) 2016 SENSORO Co.,Ltd. All Rights Reserved.
 *
 */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "sdk_common.h"
#include "app_error.h"
#include "nrf_log.h"

#include "ble_pwd.h"


/**@brief Add password characteristic.
 *
 * @param[in]   p_strm        Service structure.
 *
 * @return      NRF_SUCCESS on success, otherwise an error code.
 */
static uint32_t pwd_pwd_char_add(ble_pwd_t* p_pwd , const ble_pwd_init_t* p_pwd_init )
{
    //

    ble_gatts_char_md_t char_md;
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          ble_uuid;
    ble_gatts_attr_md_t attr_md;

    memset(&char_md, 0, sizeof(char_md));

    char_md.char_props.read   = 1;
    char_md.char_props.write  = 0;
    char_md.char_props.indicate = 0;
    char_md.p_char_user_desc  = NULL;
    char_md.p_char_pf         = NULL;
    char_md.p_user_desc_md    = NULL;
    char_md.p_cccd_md         = NULL;
    char_md.p_sccd_md         = NULL;

    ble_uuid.type = p_pwd->uuid_type;
    ble_uuid.uuid = PWD_UUID_READ_CHAR;

    memset(&attr_md, 0, sizeof(attr_md));

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
//    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.write_perm);

    attr_md.vloc       = BLE_GATTS_VLOC_STACK;
    attr_md.rd_auth    = 0;
    attr_md.wr_auth    = 1;
    attr_md.vlen       = 0;

    memset(&attr_char_value, 0, sizeof(attr_char_value));

    attr_char_value.p_uuid       = &ble_uuid;
    attr_char_value.p_attr_md    = &attr_md;
    attr_char_value.init_len     = 16 ;  //sizeof(p_pwd_init->version);
    attr_char_value.init_offs    = 0  ;
    attr_char_value.max_len      = 16 ;  //sizeof(p_pwd_init->version);
    attr_char_value.p_value      = p_pwd_init->version ;

    return sd_ble_gatts_characteristic_add(p_pwd->service_handle, 
                                           &char_md,
                                           &attr_char_value,
                                           &p_pwd->pwd_char_handle);
}



/**
 * @brief 密码服务的初始化
 */
uint32_t ble_pwd_init( ble_pwd_t* p_pwd , const ble_pwd_init_t* p_pwd_init )
{
    uint32_t        err_code;
    ble_uuid_t      service_uuid;
    ble_uuid128_t   base_uuid =
    {
        .uuid128 = PWD_BASE_UUID,
    };
    uint8_t uuid_type;

    err_code = sd_ble_uuid_vs_add( &base_uuid, &uuid_type );
    APP_ERROR_CHECK( err_code );


    p_pwd->uuid_type   = uuid_type ;
    p_pwd->conn_handle = BLE_CONN_HANDLE_INVALID;

    p_pwd->read_handler = p_pwd_init->read_handler ;
    p_pwd->error_handler = p_pwd_init->error_handler ;

    service_uuid.type = uuid_type ;
    service_uuid.uuid = PWD_UUID_SERVICE ;

    /*添加服务*/
    err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY, &service_uuid, &p_pwd->service_handle);
    VERIFY_SUCCESS(err_code);

    /*添加密码特征*/
    err_code = pwd_pwd_char_add(p_pwd , p_pwd_init);
    VERIFY_SUCCESS(err_code);

    return NRF_SUCCESS; 
}





/**@brief Authorized Read/Write event handler
 *
 * @param[in]  p_strm     Service structure.
 * @param[in]  p_ble_evt  Event received from the BLE stack.
 */
static void on_rw_auth_req(ble_pwd_t* p_pwd, ble_evt_t* p_ble_evt)
{
    uint8_t        type;
    uint16_t       handle;
#if 0
    ble_gatts_evt_rw_authorize_request_t *p_authorize_request;
    p_authorize_request     = &(p_ble_evt->evt.gatts_evt.params.authorize_request);
    type                    = p_authorize_request->type;
#endif
    type = p_ble_evt->evt.gatts_evt.params.authorize_request.type ;

    if(type == BLE_GATTS_AUTHORIZE_TYPE_READ)
    {
        handle   = p_ble_evt->evt.gatts_evt.params.authorize_request.request.read.handle ;

        if( handle == p_pwd->conn_handle )              //通过那个特征rsp的
        {
            p_pwd->read_handler( p_pwd , BLE_PWD_EVT_DATA_READ , handle );
        }
    }
    else
    {
        /*Do nothing*/
    }
}




/**
 *@brief 事件派发 
 */
void ble_pwd_on_ble_evt( ble_pwd_t* p_pwd , ble_evt_t* p_ble_evt )
{
    switch(p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_CONNECTED:
            NRF_LOG_INFO("ble_pwd ==> BLE_GAP_EVT_CONNECTED"); 

            break;

        case BLE_GAP_EVT_DISCONNECTED:
            NRF_LOG_INFO("ble_pwd ==> BLE_GAP_EVT_DISCONNECTED"); 

            break;

        case BLE_GATTS_EVT_RW_AUTHORIZE_REQUEST :
            NRF_LOG_INFO("ble_pwd ==> BLE_GATTS_EVT_RW_AUTHORIZE_REQUEST"); 
            on_rw_auth_req( p_pwd , p_ble_evt );
            break;

        case BLE_GATTS_EVT_HVC:
            NRF_LOG_INFO("ble_pwd ==> BLE_GATTS_EVT_HVC"); 

            break;

        default:
            break;
    }
}










