/* Copyright (c) 2016 SENSORO Co.,Ltd. All Rights Reserved.
 *
 */

#ifndef __BLE_STRM_HANDLE_H
#define __BLE_STRM_HANDLE_H

#include "ble.h"

#define DATA_BUFFER_SIZE     512

typedef enum
{
    BLE_SRV_STRM_DATA_CFG,
    BLE_SRV_STRM_DATA_TEST,
    BLE_SRV_STRM_DATA_CMD,
} ble_srv_strm_data_type_t;

typedef void (*strm_on_write_handler_t)(uint8_t*                 p_data,
                                  uint16_t                 len,
                                  ble_srv_strm_data_type_t data_type);

typedef void (*strm_on_read_handler_t)(uint8_t* p_data, uint16_t* p_len);

typedef void (*strm_on_sent_handler_t)(void);


/**@brief 服务初始化函数
 *
 * @param
 */
void ble_srv_strm_init(void);

/**@brief ble 事件处理函数
 *
 * @param
 */
void on_ble_srv_strm_evt(ble_evt_t* p_ble_evt);

/**@brief 注册收发回调函数
 *
 * @param
 */
void ble_srv_strm_handler_reg(strm_on_read_handler_t on_read_handler, strm_on_write_handler_t on_write_handler);


/**@brief 向主设备发送数据
 *
 * @param
 */
void ble_srv_strm_data_send(uint8_t* p_data, uint16_t len, strm_on_sent_handler_t handler);


/**@brief 向主设备发送测试数据
 *
 * @param
 */
void ble_srv_strm_test_data_send(uint8_t* p_data, uint16_t len, strm_on_sent_handler_t handler);


/**@brief 设置鉴权密码
 *
 * @param
 */
void ble_srv_strm_set_passwd(uint8_t* passwd, uint8_t len);


void ble_srv_strm_rx_timer_init(void);


/**
 * @brief 
 */
uint16_t strm_key_char_value_handle_get(void);

void key_pointer_register(uint8_t *p_key );

#endif
