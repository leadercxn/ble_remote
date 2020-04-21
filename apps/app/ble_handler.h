/* Copyright (c) 2020 SENSORO Co.,Ltd. All Rights Reserved.
 *
 */

#ifndef BLE_HANDLER_H__
#define BLE_HANDLER_H__

void ble_init(void);

void ble_advertising_scan_rsp_update(uint8_t * p_data, uint16_t size);

#endif
