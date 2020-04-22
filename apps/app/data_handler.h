#ifndef __DATA_HANDLER_H__
#define __DATA_HANDLER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "ble_strm_handler.h"

#define ADV_SCANDATA_LEN    29
#define REMOTE_UUID         0x77DB            //遥控器作为扫描的UUID   

typedef enum 
{
    GUARD_HOME_MSG = 0x55,
    ALARM_SET_MSG = 0x23 ,
    GUARD_SET_MSG = 0x68 ,
    GUARD_CANCEL_MSG = 0x71 ,
    PAIR_MSG = 0x1A ,
    NORMAL_MSG = 0x01 ,
} adv_enum_e;


/**
 * @brief 打包扫描响应包
 * 
 * @param adv_type[in]   遥控器键值
 * @param scandata[out]  返回的数据缓存
 */
void  scandata_encode( adv_enum_e adv_type  , uint8_t *scandata );

void on_ble_strm_write_handler(uint8_t * p_data, uint16_t len, ble_srv_strm_data_type_t data_type);
void on_ble_strm_read_handler(uint8_t * p_buff, uint16_t * p_len);



#ifdef _cplusplus
}
#endif

#endif



