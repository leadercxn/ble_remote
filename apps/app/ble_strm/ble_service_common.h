#ifndef  BLE_SERVICE_COMMON_H
#define  BLE_SERVICE_COMMON_H

#include "ble.h"
#include "ble_srv_common.h"


uint32_t ble_gatts_w_authorize_reply(uint32_t conn_handle, uint16_t gatt_status);
uint32_t ble_gatts_r_authorize_reply(uint32_t conn_handle, uint16_t gatt_status, uint8_t update, uint8_t* p_data, uint8_t len);

void error_handler(uint32_t nrf_error);

#endif
