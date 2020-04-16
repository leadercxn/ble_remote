/* Copyright (c) 2016 SENSORO Co.,Ltd. All Rights Reserved.
 *
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "app_error.h"
#include "app_scheduler.h"
#include "app_timer.h"

#include "ble_service_common.h"
#include "ble_hci.h"

#include "ble_strm.h"

#include "ble_strm_handler.h"

#define STRM_DATA_HEADER_LEN        2
#define STRM_DATA_LENGTH_OFFSET     0
#define STRM_DATA_OFFSET            2


#define STRM_STATE_IDLE             0
#define STRM_STATE_RECV             1
#define STRM_STATE_PROC             2


static ble_strm_t               m_strm;

static uint16_t                 m_rx_exp_len = 0;
static uint8_t                  m_rx_buffer[DATA_BUFFER_SIZE];
static uint16_t                 m_rx_buffer_len = 0;

static uint8_t                  m_strm_state = STRM_STATE_IDLE;

static bool                     m_is_locked = false;
static uint8_t                  m_passwd[16];
static bool                     m_is_passwd_set = false;

static uint8_t                  m_tx_buff[DATA_BUFFER_SIZE];
static uint16_t                 m_tx_buff_len = 0;;
static uint16_t                 m_tx_offset = 0;

static bool                     m_is_tx_processing = false;

static strm_on_read_handler_t   m_on_read_handler = NULL;
static strm_on_write_handler_t  m_on_write_handler = NULL;
static strm_on_sent_handler_t   m_on_sent_handler = NULL;

#define RX_TIMEOUT_TIME         10000
APP_TIMER_DEF(m_rx_timer);


/**@brief 数据处理调度函数
 *
 * @param
 */
static void rx_sched_evt(void* p_event_data, uint16_t event_size)
{
    ble_srv_strm_data_type_t data_type = *(ble_srv_strm_data_type_t*)p_event_data;
    if(m_on_write_handler != NULL)
    {
        m_on_write_handler(m_rx_buffer, m_rx_buffer_len, data_type);
    }

    m_strm_state = STRM_STATE_IDLE;
    m_rx_exp_len = 0;
    m_rx_buffer_len = 0;
}

/**@brief 将长包分成多个20 bytes 的小包 indication
 *
 * @param
 *
 * @retval
 */
static uint32_t ble_srv_strm_indicate(ble_strm_t*               p_strm,
                                      ble_gatts_char_handles_t  char_handle,
                                      uint8_t*                  p_data,
                                      uint16_t                  len,
                                      uint16_t*                 p_offset)
{
    uint32_t err_code = NRF_SUCCESS;
    uint8_t indication_len = 0;
    uint16_t offset = *p_offset;

    if(*p_offset >= len)
    {
        m_is_tx_processing = false;
        if(m_on_sent_handler != NULL)
        {
            m_on_sent_handler();
            m_on_sent_handler = NULL;
        }
        return err_code;
    }

    if((len - offset) > 20)
    {
        indication_len = 20;
    }
    else
    {
        indication_len = len - offset;
    }

    err_code = ble_strm_char_indicate(p_strm, char_handle, &p_data[offset], indication_len);
    *p_offset = offset + indication_len;

    return err_code;
}

/**@brief 写数据回调函数
 *
 * @param
 */
static void write_handler(ble_strm_t*         p_strm,
                          ble_strm_evt_type_t evt_type,
                          uint16_t            value_handle,
                          uint8_t*            p_data,
                          uint16_t            length)
{
    uint32_t err_code;

    ble_gatts_rw_authorize_reply_params_t reply =
    {
        .type                        = BLE_GATTS_AUTHORIZE_TYPE_WRITE,
        .params.write.gatt_status    = BLE_GATT_STATUS_SUCCESS,
        .params.write.update         = 1,
        .params.write.offset         = 0,
        .params.write.len            = 0,
        .params.write.p_data         = NULL
    };
    switch(evt_type)
    {

        case BLE_STRM_EVT_DATA_WRITE:
        case BLE_STRM_EVT_TEST_WRITE:

            /*如果还没有鉴权，返回WRITE_NOT_PERMITTED*/
            if(m_is_locked)
            {
                reply.params.write.gatt_status = BLE_GATT_STATUS_ATTERR_WRITE_NOT_PERMITTED;
                break;
            }

        case BLE_STRM_EVT_CMD_WRITE:
            /*如果正在处理数据中，返回WRITE_NOT_PERMITTED*/
            if(STRM_STATE_PROC == m_strm_state)
            {
                reply.params.write.gatt_status = BLE_GATT_STATUS_ATTERR_WRITE_NOT_PERMITTED;
                break;
            }
            else if(STRM_STATE_IDLE == m_strm_state)
            {
                /*如果长度过短，小于header长度，返回WRITE_NOT_PERMITTED*/
                if(length < STRM_DATA_HEADER_LEN)
                {
                    reply.params.write.gatt_status = BLE_GATT_STATUS_ATTERR_WRITE_NOT_PERMITTED;
                    break;
                }

                /*解析长度*/
                m_rx_exp_len = uint16_decode(&p_data[STRM_DATA_LENGTH_OFFSET]);

                if(m_rx_exp_len != 0)
                {
                    m_strm_state = STRM_STATE_RECV;

                    memset(m_rx_buffer, 0, sizeof(m_rx_buffer));
                    memcpy(&m_rx_buffer[m_rx_buffer_len], &p_data[STRM_DATA_HEADER_LEN], length - STRM_DATA_HEADER_LEN);
                    m_rx_buffer_len += length - STRM_DATA_HEADER_LEN;

                    //                timer_start( m_rx_timer, RX_TIMEOUT_TIME );
                }
            }
            else if(STRM_STATE_RECV == m_strm_state)
            {
                uint16_t len = ((m_rx_buffer_len + length) <= m_rx_exp_len) ? length : (m_rx_exp_len - m_rx_buffer_len);

                memcpy(&m_rx_buffer[m_rx_buffer_len], p_data, len);
                m_rx_buffer_len += len;
            }

            reply.params.write.len = length;
            reply.params.write.p_data = p_data;

            /*如果数据收完了，处理数据*/
            if(m_rx_buffer_len >= m_rx_exp_len)
            {
                ble_srv_strm_data_type_t data_type = BLE_SRV_STRM_DATA_CFG;

                //            timer_stop( m_rx_timer );
                
                m_strm_state = STRM_STATE_PROC;
                if(BLE_STRM_EVT_DATA_WRITE == evt_type)
                {
                    data_type = BLE_SRV_STRM_DATA_CFG;
                }
                else if(BLE_STRM_EVT_TEST_WRITE == evt_type)
                {
                    data_type = BLE_SRV_STRM_DATA_TEST;
                }
                else
                {
                    data_type = BLE_SRV_STRM_DATA_CMD;
                }
                app_sched_event_put(&data_type, sizeof(ble_srv_strm_data_type_t), rx_sched_evt);
            }
            break;

        case BLE_STRM_EVT_PASSWORD_WRITE:
            if(m_is_locked)
            {
                if(memcmp(p_data, m_passwd , sizeof(m_passwd)) == 0)
                {
                    m_is_locked = false;
                }
                else
                {
                    reply.params.write.gatt_status = BLE_GATT_STATUS_ATTERR_WRITE_NOT_PERMITTED;
                }
            }

            reply.params.write.len = length;
            reply.params.write.p_data = p_data;
            break;

        default:
            break;

    }

    err_code = sd_ble_gatts_rw_authorize_reply(p_strm->conn_handle, &reply);
    APP_ERROR_CHECK(err_code);
}

/**@brief 读数据回调函数
 *
 * @param
 */
static void read_handler(ble_strm_t*           p_strm,
                         ble_strm_evt_type_t   evt_type,
                         uint16_t              value_handle)
{
    uint32_t err_code;
    bool is_data_rest = false;

    ble_gatts_rw_authorize_reply_params_t reply =
    {
        .type                       = BLE_GATTS_AUTHORIZE_TYPE_READ,
        .params.read.gatt_status    = BLE_GATT_STATUS_SUCCESS,
        .params.read.update         = 1,
        .params.read.offset         = 0,
        .params.read.len            = 0,
        .params.read.p_data         = NULL
    };

    switch(evt_type)
    {
        case BLE_STRM_EVT_DATA_READ:
            /*如果还没有鉴权，返回READ_NOT_PERMITTED*/
            if(m_is_locked)
            {
                reply.params.read.gatt_status = BLE_GATT_STATUS_ATTERR_READ_NOT_PERMITTED;
                break;
            }

            if(m_is_tx_processing)
            {
                reply.params.read.gatt_status = BLE_GATT_STATUS_ATTERR_READ_NOT_PERMITTED;
                break;
            }

            memset(m_tx_buff, 0, sizeof(m_tx_buff));
            m_tx_buff_len = 0;
            m_tx_offset = 0;

            if(m_on_read_handler != NULL)
            {
                m_on_read_handler(&m_tx_buff[STRM_DATA_HEADER_LEN], &m_tx_buff_len);

                /*给数据带上协议头，包括数据长度*/
                uint16_encode(m_tx_buff_len, m_tx_buff);
                m_tx_buff_len += 2;

                /*如果长度小于等于20，直接通过read返回数据*/
                if(m_tx_buff_len <= 20)
                {
                    reply.params.read.len            = m_tx_buff_len;
                    reply.params.read.p_data         = m_tx_buff;
                }

                /*如果长度小于等于20，先通过read返回20bytes，剩下的数据通过indication返回*/
                else
                {
                    reply.params.read.len            = 20;
                    reply.params.read.p_data         = m_tx_buff;

                    is_data_rest = true;
                    m_tx_offset += 20;
                }
            }
            else
            {
                reply.params.read.gatt_status = BLE_GATT_STATUS_UNKNOWN;
            }
            break;

        default:
            break;
    }

    err_code = sd_ble_gatts_rw_authorize_reply(p_strm->conn_handle, &reply);
    APP_ERROR_CHECK(err_code);

    if(is_data_rest)
    {
        is_data_rest = false;
        m_is_tx_processing = true;
        err_code = ble_srv_strm_indicate(p_strm, p_strm->data_char_handle, m_tx_buff, m_tx_buff_len, &m_tx_offset);
        if(NRF_SUCCESS != err_code)
        {
            m_is_tx_processing = false;
        }
    }
}


/**@brief 读数据回调函数
 *
 * @param
 */
static void hvc_handler(ble_strm_t*                p_strm,
                        ble_gatts_char_handles_t   char_handle)
{
    uint32_t err_code;
    err_code = ble_srv_strm_indicate(p_strm, char_handle, m_tx_buff, m_tx_buff_len, &m_tx_offset);
    if(NRF_SUCCESS != err_code)
    {
        m_is_tx_processing = false;
    }
}

/**@brief ble 事件处理函数
 *
 * @param
 */
static void ble_srv_strm_on_ble_evt(ble_strm_t* p_strm, ble_evt_t* p_ble_evt)
{
    switch(p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_DISCONNECTED:
            m_rx_exp_len = 0;
            m_rx_buffer_len = 0;
            m_strm_state = STRM_STATE_IDLE;

            m_tx_buff_len = 0;
            m_tx_offset = 0;

            m_is_tx_processing = false;

            if(m_is_passwd_set)
            {
                m_is_locked = true;
            }
            break;
    }
}

void on_ble_srv_strm_evt(ble_evt_t* p_ble_evt)
{
    ble_strm_on_ble_evt(&m_strm, p_ble_evt);
    ble_srv_strm_on_ble_evt(&m_strm, p_ble_evt);
}

void ble_srv_strm_init(void)
{
    uint32_t err_code;
    ble_strm_init_t init;

    init.write_handler      = write_handler;
    init.read_handler       = read_handler;
    init.hvc_handler        = hvc_handler;
    init.error_handler      = NULL;
    // init.version            = (uint8_t *)FW_VERSION_FULL;

    err_code = ble_strm_init(&m_strm, &init);
    APP_ERROR_CHECK(err_code);
}


void ble_srv_strm_handler_reg(strm_on_read_handler_t on_read_handler, strm_on_write_handler_t on_write_handler)
{
    m_on_read_handler = on_read_handler;
    m_on_write_handler = on_write_handler;
}


void ble_srv_strm_data_send(uint8_t* p_data, uint16_t len, strm_on_sent_handler_t handler)
{
    uint32_t err_code;
    if(BLE_CONN_HANDLE_INVALID != m_strm.conn_handle)
    {
        m_on_sent_handler = handler;

        memset(m_tx_buff, 0, sizeof(m_tx_buff));
        m_tx_buff_len = 0;
        m_tx_offset = 0;

        memcpy(&m_tx_buff[STRM_DATA_OFFSET], p_data, len);
        m_tx_buff_len += len;
        uint16_encode(m_tx_buff_len, m_tx_buff);
        m_tx_buff_len += 2;

        m_is_tx_processing = true;
        err_code = ble_srv_strm_indicate(&m_strm, m_strm.data_char_handle, m_tx_buff, m_tx_buff_len, &m_tx_offset);
        if(NRF_SUCCESS != err_code)
        {
            m_is_tx_processing = false;
        }
    }
}

void ble_srv_strm_test_data_send(uint8_t* p_data, uint16_t len, strm_on_sent_handler_t handler)
{
    uint32_t err_code;
    if(BLE_CONN_HANDLE_INVALID != m_strm.conn_handle)
    {
        m_on_sent_handler = handler;

        memset(m_tx_buff, 0, sizeof(m_tx_buff));
        m_tx_buff_len = 0;
        m_tx_offset = 0;

        memcpy(&m_tx_buff[STRM_DATA_OFFSET], p_data, len);
        m_tx_buff_len += len;
        uint16_encode(m_tx_buff_len, m_tx_buff);
        m_tx_buff_len += 2;

        m_is_tx_processing = true;
        err_code = ble_srv_strm_indicate(&m_strm, m_strm.test_char_handle, m_tx_buff, m_tx_buff_len, &m_tx_offset);
        if(NRF_SUCCESS != err_code)
        {
            m_is_tx_processing = false;
        }
    }
}


void ble_srv_strm_set_passwd(uint8_t* passwd, uint8_t len)
{
    APP_ERROR_CHECK_BOOL(len == 16);
    memcpy(m_passwd, passwd, 16);
    m_is_locked = true;
    m_is_passwd_set = true;
}

static void ble_srv_strm_rx_timer_handler(void* p_context)
{
    uint32_t err_code;
    if(BLE_CONN_HANDLE_INVALID != m_strm.conn_handle)
    {
        err_code =  sd_ble_gap_disconnect(m_strm.conn_handle, BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
        APP_ERROR_CHECK(err_code);
    }
}

void ble_srv_strm_rx_timer_init(void)
{
    uint32_t err_code;

    err_code = app_timer_create(&m_rx_timer, APP_TIMER_MODE_SINGLE_SHOT, ble_srv_strm_rx_timer_handler);
    APP_ERROR_CHECK(err_code);

    //    timer_create( &m_rx_timer, APP_TIMER_MODE_SINGLE_SHOT, strm_rx_timer_handler );
}
