#include <stdint.h>
#include <string.h>

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#include "fstorage_handler.h"
#include "aes_handler.h"
#include "data_handler.h"
#include "battery_handler.h"
#include "ble_strm_handler.h"
#include "version.h"
#include "pb_common.h"
#include "pb_decode.h"
#include "pb_encode.h"
#include "msgnode.pb.h"
#include "ble_dfu.h"
#include "nrf_drv_rng.h"

#define ALPHA_TOOL_CFG_VERSION      0X05

static uint8_t scret_data[16];                //最终暗文数据16B

#define NUM_OF_TX_POWER                 8
#define TXP_ARRAY                       {4,    0,     -4,    -8, -12,   -16,  -20,  -30}

#define INTERVAL_MAX                    200
#define INTERVAL_MIN                    20

/**
 * @brief scret_data 准备
 */
static void scret_data_ready( adv_enum_e adv_type )
{
    memset( scret_data , 0 , sizeof(scret_data) );

    scret_data[0] = adv_type ;

    NRF_LOG_DEBUG("scret_data_ready  : " );
    NRF_LOG_HEXDUMP_DEBUG(scret_data , sizeof(scret_data) );     //打印数据
}

/**
 * @brief scret_data 打包
 */
static void scret_data_encode( void )
{
    for( uint8_t i = 0 ; i < sizeof(scret_data) ; i++ )
    {
        scret_data[i] ^= cc_ecb_data.ciphertext[i] ;
    }
    NRF_LOG_DEBUG("scret_data_encode  : " );
    NRF_LOG_HEXDUMP_DEBUG(scret_data , sizeof(scret_data) );     //打印数据
}



/**
 * @brief 打包扫描响应包
 * 
 * @param adv_type[in]   遥控器键值
 * @param scandata[out]  返回的数据缓存
 */
void  scandata_encode( adv_enum_e adv_type  , uint8_t *scandata )
{
    uint8_t index = 0 ;
    uint16_t battery_volt = battery_sample();

    memset( scandata , 0 , ADV_SCANDATA_LEN );

    scret_data_ready( adv_type );       //
    aes_encrypt();                      //cleartext && key 加密
    scret_data_encode();                //生成最终暗文

    scandata[index] = ( ADV_SCANDATA_LEN - 1 ) ;
    index++;

    scandata[index] = 0x16 ;
    index++;

    uint16_encode( REMOTE_UUID , &scandata[index] ); 
    index+=2;

    scandata[index] = APP_VERSION_BYTE ;
    index++;

    uint16_encode( g_adv_fnt , &scandata[index] ); 
    index+=2;

    scandata[index] = rng_get() ;
    index++;

    scandata[index] = adv_type ;
    index++;

    memcpy( &scandata[index] , &g_user_param.sn[0] , 2 );
    index+=2;

    uint16_encode( battery_volt , &scandata[index] );   //电池电压
    index+=2;

    memcpy( &scandata[index] , scret_data , sizeof(scret_data) );
    index+=sizeof(scret_data);

    NRF_LOG_DEBUG("scandata encode finish , scandata len = %d  : "  , index );
    NRF_LOG_HEXDUMP_DEBUG(scandata , index );     //打印数据
}


/**
 * @brief       打包key特征的数据
 * @param[in]   指向key特征的特征数据指针
 */
void key_char_attr_data_encode ( uint8_t *attr_data )
{
    static nrf_ecb_hal_data_t cc_ecb_data ;

    uint16_t nonce ;
    nrf_drv_rng_rand( (uint8_t *)&nonce , sizeof(uint16_t) );

     NRF_LOG_INFO("nonce = 0x%04x "  , nonce );

    memset( &cc_ecb_data , 0 , sizeof(nrf_ecb_hal_data_t) );                                    

    memcpy( cc_ecb_data.cleartext , g_user_param.appSecureKey , sizeof(cc_ecb_data.cleartext)   );
    memcpy( cc_ecb_data.key , &nonce , sizeof( uint16_t ) );
    
    sd_ecb_block_encrypt(&cc_ecb_data);

    NRF_LOG_INFO("encrypt.ciphertext: " );
    NRF_LOG_HEXDUMP_INFO(cc_ecb_data.ciphertext, sizeof(cc_ecb_data.ciphertext));           //打印数据

    memcpy( attr_data , cc_ecb_data.ciphertext , sizeof(cc_ecb_data.ciphertext) );
    memcpy( &attr_data[16] , &nonce , sizeof(uint16_t) );


    //sd_ble_gatts_value_set(uint16_t conn_handle, uint16_t handle, ble_gatts_value_t *p_value);
}





/**
 * @brief 校验BLE发射功率的有效性
 */
bool ble_tx_power_check(int8_t txp)
{
    int8_t txp_array[NUM_OF_TX_POWER] = TXP_ARRAY;

    for(uint8_t i = 0; i < NUM_OF_TX_POWER; i++)
    {
        if(txp_array[i] == txp)
        {
            return true;
        }
    }
    return false;
}

/**
 * @brief 校验BLE间隔有效性
 */
bool ble_interval_check(float intv)
{
    if(intv > INTERVAL_MAX || intv < INTERVAL_MIN)
    {
        return false;
    }
    return true;
}

/**
 * @brief ble  接收到的数据验证
 */
static uint8_t msg_cfg_verify(MsgNode *p_msg)
{
    ret_code_t  err_code = NRF_SUCCESS ;

    if( p_msg->has_bleParam )       //包含有ble的参数
    {
        if( p_msg->bleParam.has_bleInterval )
        {
            NRF_LOG_INFO("rx ble_interval : %d " , p_msg->bleParam.bleInterval );
            if(!ble_interval_check(p_msg->bleParam.bleInterval))
            {
                return NRF_ERROR_INVALID_DATA ;
            }
        }

        if( p_msg->bleParam.has_bleTxp )
        {
            NRF_LOG_INFO("rx bleTxp : %d " , p_msg->bleParam.bleTxp );
            if(!ble_tx_power_check(p_msg->bleParam.bleTxp))
            {
                return NRF_ERROR_INVALID_DATA ;
            }
        }

    }
    return err_code;
}

/**
 * @brief 接收到的数据处理
 */
static void msg_cfg_handler(MsgNode *p_msg)
{
    bool is_update_to_flash = false ;
    bool is_fac_reset = false;
    bool is_dfu = false;
    if( p_msg->has_bleParam )       //包含有ble的参数
    {
        if( p_msg->bleParam.has_bleInterval )
        {
            uint16_t bleInterval = (uint16_t)p_msg->bleParam.bleInterval;

            if( g_user_param.ble_int !=  bleInterval )  //产生数据更新
            {
                g_user_param.ble_int = bleInterval;
                is_update_to_flash = true ; 
            }
        }

        if( p_msg->bleParam.bleTxp )
        {
            if( g_user_param.ble_txp != p_msg->bleParam.bleTxp )
            {
                g_user_param.ble_txp = p_msg->bleParam.bleTxp;
                is_update_to_flash = true ; 
            }
        }
    }

    if(p_msg->has_appParam)
    {
        if(p_msg->appParam.has_cmd)
        {
            NRF_LOG_INFO("appParam.cmd = %d\n", p_msg->appParam.cmd);
            switch(p_msg->appParam.cmd)
            {
                case AppCmd_APP_CMD_FAC_RESET:
                    is_fac_reset = true;
                    break;
                case AppCmd_APP_CMD_DFU:
                    is_dfu = true;
                    break;
                default:
                    break;
            }
        }

    }

    if( true == is_update_to_flash)     //数据更新到flash区
    {
        NRF_LOG_INFO("data update to flash, bleInterval =%d ,bleTxp =%d " , g_user_param.ble_int , g_user_param.ble_txp );
        user_param_store_iflash();
    }

    if(is_dfu)
    {
        is_dfu = false;
        NRF_LOG_INFO("go into dfu bootloader\n " , g_user_param.ble_int , g_user_param.ble_txp );

        uint32_t err_code;
        err_code = ble_dfu_buttonless_bootloader_start_prepare();
        if (err_code != NRF_SUCCESS)
        {
            NRF_LOG_INFO("dfu error \n"); 
        }

    }

}

/**
 * @brief ble  发送的数据填充
 */
uint8_t fill_msgnode_storage(MsgNode * p_msg , storage_user_param_t *p_user_param)
{
    ret_code_t  err_code = NRF_SUCCESS ;

    if(p_msg == NULL)
    {
        return NRF_ERROR_INVALID_PARAM;
    }

    if( p_user_param )
    {
        p_msg->has_bleParam = true ;

        p_msg->bleParam.has_bleInterval = true ;
        p_msg->bleParam.bleInterval = (float)(p_user_param->ble_int);

        p_msg->bleParam.has_bleTxp = true ;
        p_msg->bleParam.bleTxp = (int32_t)p_user_param->ble_txp ;

        p_msg->has_appBleFnt = true ;
        p_msg->appBleFnt = p_user_param->blefnt;        //手机端app已经无更新，新字段无法添加

    }

    return  err_code  ;
}


void on_ble_strm_write_handler(uint8_t * p_data,
                               uint16_t len,
                               ble_srv_strm_data_type_t data_type)
{

    ret_code_t  err_code = NRF_SUCCESS ;
    bool        status;
    uint8_t     *p_proto = &p_data[1];
    uint16_t    proto_len = len - 1;
    
    pb_istream_t stream;
    MsgNode msg_node = MsgNode_init_zero;

    NRF_LOG_INFO("ble rx data :" );
    NRF_LOG_HEXDUMP_INFO( p_data , len );     //打印数据

    switch( data_type )
    {
        case BLE_SRV_STRM_DATA_CFG :                                                    //数据配置
                stream = pb_istream_from_buffer(p_proto, proto_len);                    //解析数据
                status = pb_decode(&stream, MsgNode_fields, &msg_node);
                if(status)
                {

                    NRF_LOG_INFO("app data decode success  " );
                    err_code = msg_cfg_verify(&msg_node);

                }
                else
                {
                    err_code = NRF_ERROR_INVALID_STATE  ;
                    NRF_LOG_ERROR("app data decode fail  " );
                }
                
                uint8_t ret_buff[2] ;
                ret_buff[0] = ALPHA_TOOL_CFG_VERSION ;
                ret_buff[1] = err_code ;
                
                ble_srv_strm_data_send(ret_buff, sizeof(ret_buff), NULL);               //ble 发送数据返回到app

                // 数据处理
                if( NRF_SUCCESS == err_code )
                {
                    msg_cfg_handler(&msg_node);
                }
        break;
    }
}

void on_ble_strm_read_handler(uint8_t * p_buff, uint16_t * p_len)
{
    bool        status;
    uint16_t    len = 0;
    MsgNode     msg_node = MsgNode_init_zero;

    //填充数据
    fill_msgnode_storage(&msg_node , &g_user_param);

    pb_ostream_t stream = pb_ostream_from_buffer(p_buff + 1, 512);                      // 打包数据
    status = pb_encode(&stream, MsgNode_fields, &msg_node);

    if(status)
    {
        len = stream.bytes_written ;

        p_buff[0] = ALPHA_TOOL_CFG_VERSION;                                             // 此处的flag不能乱改
        len++;

        *p_len = len;
        NRF_LOG_DEBUG("app data encode success : " );
        NRF_LOG_HEXDUMP_DEBUG(p_buff , len );                                            // 打印数据
    }
    else
    {
        NRF_LOG_ERROR("app data encode fail  " );
    }

}



