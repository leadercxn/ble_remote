/* Copyright (c) 2016 SENSORO Co.,Ltd. All Rights Reserved.
 *
 */

#ifndef BLE_STRM_H__
#define BLE_STRM_H__

#define CHAR_VALUE_LEN_MAX                      20

#define STRM_BASE_UUID {0xE4,0xE0,0xCA,0x0C,0xA3,0x50,0x4A,0x83,0xA2,0x1B,0x4E,0x7A,0x29,0x69,0xAE,0xDE }

#define STRM_UUID_SERVICE                       0x0300
#define STRM_UUID_DATA_CHAR                     0X0301
#define STRM_UUID_PWD_CHAR                      0X0302
#define STRM_UUID_TEST_CHAR                     0X0303
#define STRM_UUID_CMD_CHAR                      0X0304
#define STRM_UUID_VERSION_CHAR                  0X03FF

typedef enum
{
    BLE_STRM_EVT_DATA_WRITE,
    BLE_STRM_EVT_LONG_DATA_WRITE,
    BLE_STRM_EVT_PASSWORD_WRITE,
    BLE_STRM_EVT_TEST_WRITE,
    BLE_STRM_EVT_CMD_WRITE,

    BLE_STRM_EVT_DATA_READ,
} ble_strm_evt_type_t;

// Forward declaration of the ble_strm_t type.
typedef struct ble_strm_s ble_strm_t;

/**@brief Beacon Configuration Service event handler type. */
typedef void (*ble_strm_write_handler_t)(ble_strm_t*           p_strm,
                                         ble_strm_evt_type_t   evt_type,
                                         uint16_t              value_handle,
                                         uint8_t*              p_data,
                                         uint16_t              length);

typedef void (*ble_strm_read_handler_t)(ble_strm_t*           p_strm,
                                        ble_strm_evt_type_t   evt_type,
                                        uint16_t              value_handle);

typedef void (*ble_strm_hvc_handler_t)(ble_strm_t*                p_strm,
                                       ble_gatts_char_handles_t   char_handle);

typedef struct
{
    ble_strm_write_handler_t    write_handler;
    ble_strm_read_handler_t     read_handler;
    ble_strm_hvc_handler_t      hvc_handler;
    ble_srv_error_handler_t     error_handler;
    uint8_t                   * version;
} ble_strm_init_t;


/**@brief Beacon Configuration Service structure. This contains various status information for the service. */
typedef struct ble_strm_s
{
    uint8_t                      uuid_type;
    uint16_t                     conn_handle;
    uint16_t                     service_handle;
    ble_gatts_char_handles_t     data_char_handle;
    ble_gatts_char_handles_t     pwd_char_handle;
    ble_gatts_char_handles_t     test_char_handle;
    ble_gatts_char_handles_t     cmd_char_handle;

    ble_strm_write_handler_t     write_handler;
    ble_strm_read_handler_t      read_handler;
    ble_strm_hvc_handler_t       hvc_handler;
    ble_srv_error_handler_t      error_handler;
} _ble_strm_s;


uint32_t ble_strm_init(ble_strm_t* p_strm, const ble_strm_init_t* p_strm_init);

void ble_strm_on_ble_evt(ble_strm_t* p_strm, ble_evt_t* p_ble_evt);

/**@brief Function for characteristic indication
 *
 * @param
 *
 * @retval
 */
uint32_t ble_strm_char_indicate(ble_strm_t*               p_strm,
                                ble_gatts_char_handles_t  handle,
                                uint8_t*                  p_data,
                                uint16_t                  length);
#endif

