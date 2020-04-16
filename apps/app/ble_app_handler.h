#ifndef __BLE_APP_HANDLER_H
#define __BLE_APP_HANDLER_H

#include "ble.h"

/**
 *@brief ble相关的初始化 
 */
void ble_init(void);

/**
 * @brief Function for starting advertising.开始广播
 */
void advertising_start(void);

/**
 * @brief 停止广播
 */
void advertising_stop(void);

/**
 * @brief Function for changing the tx power.
 */
void tx_power_set( int8_t tx_power );

/**
 * @brief 获取广播持续时间
 */
uint16_t adv_continue_time(void);

/**
 * @brief 自定义广播报文打包和配置
 */
void advertising_encode_msg_data( adv_enum_e msg_type );

#endif




