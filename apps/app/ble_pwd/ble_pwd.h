/* Copyright (c) 2016 SENSORO Co.,Ltd. All Rights Reserved.
 *
 */

#ifndef BLE_PWD_H__
#define BLE_PWD_H__

#include "ble_srv_common.h"

#define PWD_BASE_UUID {0xE4,0xE0,0xCA,0x0C,0xA3,0x50,0x4A,0x83,0xA2,0x1B,0x4E,0x7A,0x29,0x69,0xAE,0xDE }

#define PWD_UUID_SERVICE                       0x03FD
#define PWD_UUID_READ_CHAR                     0X03FE



/*pwd服务事件枚举*/
typedef enum
{
    BLE_PWD_EVT_DATA_READ,
} ble_pwd_evt_type_e;

// Forward declaration of the ble_strm_t type.
typedef struct ble_pwd_s ble_pwd_t;

/**@brief Beacon Configuration Service event handler type. */
typedef void (*ble_pwd_read_handler_t)( ble_pwd_t *           p_pwd,
                                        ble_pwd_evt_type_e    evt_type,
                                        uint16_t              value_handle );

typedef struct 
{
    ble_pwd_read_handler_t      read_handler;
    ble_srv_error_handler_t     error_handler;
    uint8_t                     *version;
} ble_pwd_init_t ;
 


/**@brief Beacon Configuration Service structure. This contains various status information for the service. */
typedef struct ble_pwd_s
{
    uint8_t                      uuid_type;
    uint16_t                     conn_handle;
    uint16_t                     service_handle;

//    ble_gatts_char_handles_t     data_char_handle;
    ble_gatts_char_handles_t     pwd_char_handle;
//    ble_gatts_char_handles_t     test_char_handle;
//    ble_gatts_char_handles_t     cmd_char_handle;

    ble_pwd_read_handler_t       read_handler;
    ble_srv_error_handler_t      error_handler;
} _ble_pwd_s ;

/**
 * @brief 密码服务的初始化
 */
uint32_t ble_pwd_init( ble_pwd_t* p_pwd , const ble_pwd_init_t* p_pwd_init );

/**
 *@brief 事件派发 
 */
void ble_pwd_on_ble_evt( ble_pwd_t* p_pwd , ble_evt_t* p_ble_evt ) ;



#endif



