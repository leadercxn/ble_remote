/* Copyright (c) 2016 SENSORO Co.,Ltd. All Rights Reserved.
 *
 */

#ifndef BLE_PWD_HANDLER_H__
#define BLE_PWD_HANDLER_H__

















/**
 *@brief 密码服务初始化 
 */
void ble_srv_pwd_init(void);



/**
 *@brief pwd的事件派发
 */
void on_ble_srv_pwd_evt(ble_evt_t* p_ble_evt);








#endif

