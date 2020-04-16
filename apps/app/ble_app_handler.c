#include <stdint.h>
#include <string.h>
#include "nordic_common.h"
#include "nrf.h"
#include "app_error.h"
#include "ble.h"
#include "ble_err.h"
#include "ble_hci.h"
#include "ble_srv_common.h"
#include "ble_advdata.h"
#include "ble_conn_params.h"
#include "nrf_sdh.h"
#include "nrf_sdh_ble.h"
#include "app_timer.h"
#include "app_button.h"
#include "app_scheduler.h"
#include "nrf_ble_gatt.h"
#include "nrf_ble_qwr.h"
#include "nrf_pwr_mgmt.h"
#include "ble_advertising.h"
#include "ble_dfu.h"
#include "nrf_bootloader_info.h"
#include "nrf_power.h"
#include "ble_conn_state.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "timer.h"

#include "battery_handler.h"
#include "fstorage_cfg.h"
#include "fstorage_handler.h"

#include "aes_handler.h"
#include "data_handler.h"

#include "ble_strm_handler.h"


#define DEVICE_NAME                     "SSS_CONTROLLER"                            /**< Name of device. Will be included in the advertising data. */

#define APP_BLE_OBSERVER_PRIO           3                                       /**< Application's BLE observer priority. You shouldn't need to modify this value. */
#define APP_BLE_CONN_CFG_TAG            1                                       /**< A tag identifying the SoftDevice BLE configuration. */

#define APP_ADV_DURATION                0                                       /**< The advertising time-out (in units of 10ms). When set to 0, we will never time out. */

#define MIN_CONN_INTERVAL               MSEC_TO_UNITS(100, UNIT_1_25_MS)        /**< Minimum acceptable connection interval (0.5 seconds). */
#define MAX_CONN_INTERVAL               MSEC_TO_UNITS(200, UNIT_1_25_MS)        /**< Maximum acceptable connection interval (1 second). */
#define SLAVE_LATENCY                   0                                       /**< Slave latency. */
#define CONN_SUP_TIMEOUT                MSEC_TO_UNITS(4000, UNIT_10_MS)         /**< Connection supervisory time-out (4 seconds). */

uint16_t m_conn_handle = BLE_CONN_HANDLE_INVALID;                               //是否有链接

static uint8_t  m_adv_handle = BLE_GAP_ADV_SET_HANDLE_NOT_SET;                  /**< Advertising handle used to identify an advertising set. */
static bool     m_adv_on_off = false ;                                          //广播打开状态

/**@brief Handler for shutdown preparation.
 *
 * @details During shutdown procedures, this function will be called at a 1 second interval
 *          untill the function returns true. When the function returns true, it means that the
 *          app is ready to reset to DFU mode.
 *
 * @param[in]   event   Power manager event.
 *
 * @retval  True if shutdown is allowed by this power manager handler, otherwise false.
 */
static bool app_shutdown_handler(nrf_pwr_mgmt_evt_t event)
{
    switch(event)
    {
        case NRF_PWR_MGMT_EVT_PREPARE_DFU:
            NRF_LOG_INFO("Power management wants to reset to DFU mode.");
            // YOUR_JOB: Get ready to reset into DFU mode
            //
            // If you aren't finished with any ongoing tasks, return "false" to
            // signal to the system that reset is impossible at this stage.
            //
            // Here is an example using a variable to delay resetting the device.
            //
            // if (!m_ready_for_reset)
            // {
            //      return false;
            // }
            // else
            //{
            //
            //    // Device ready to enter
            //    uint32_t err_code;
            //    err_code = sd_softdevice_disable();
            //    APP_ERROR_CHECK(err_code);
            //    err_code = app_timer_stop_all();
            //    APP_ERROR_CHECK(err_code);
            //}
            break;

        default:
            // YOUR_JOB: Implement any of the other events available from the power management module:
            //      -NRF_PWR_MGMT_EVT_PREPARE_SYSOFF
            //      -NRF_PWR_MGMT_EVT_PREPARE_WAKEUP
            //      -NRF_PWR_MGMT_EVT_PREPARE_RESET
            return true;
    }

    NRF_LOG_INFO("Power management allowed to reset to DFU mode.");
    return true;
}

//lint -esym(528, m_app_shutdown_handler)
/**@brief Register application shutdown handler with priority 0.
 */
NRF_PWR_MGMT_HANDLER_REGISTER(app_shutdown_handler, 0);

static void buttonless_dfu_sdh_state_observer(nrf_sdh_state_evt_t state, void *p_context)
{
    if(state == NRF_SDH_EVT_STATE_DISABLED)
    {
        // Softdevice was disabled before going into reset. Inform bootloader to skip CRC on next boot.
        nrf_power_gpregret2_set(BOOTLOADER_DFU_SKIP_CRC);

        //Go to system off.
        nrf_pwr_mgmt_shutdown(NRF_PWR_MGMT_SHUTDOWN_GOTO_SYSOFF);
    }
}

/* nrf_sdh state observer. */
NRF_SDH_STATE_OBSERVER(m_buttonless_dfu_state_obs, 0) =
{
    .handler = buttonless_dfu_sdh_state_observer,
};

static void disconnect(uint16_t conn_handle, void *p_context)
{
    UNUSED_PARAMETER(p_context);

    ret_code_t err_code = sd_ble_gap_disconnect(conn_handle, BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
    if(err_code != NRF_SUCCESS)
    {
        NRF_LOG_WARNING("Failed to disconnect connection. Connection handle: %d Error: %d", conn_handle, err_code);
    }
    else
    {
        NRF_LOG_DEBUG("Disconnected connection handle %d", conn_handle);
    }
}

/**@brief Function for handling dfu events from the Buttonless Secure DFU service
 *
 * @param[in]   event   Event from the Buttonless Secure DFU service.
 */
static void ble_dfu_evt_handler(ble_dfu_buttonless_evt_type_t event)
{
    switch(event)
    {
        case BLE_DFU_EVT_BOOTLOADER_ENTER_PREPARE:
        {
            NRF_LOG_INFO("Device is preparing to enter bootloader mode.");

            // YOUR_JOB: Disconnect all other bonded devices that currently are connected.
            //         This is required to receive a service changed indication
            //         on bootup after a successful (or aborted) Device Firmware Update.
            uint32_t conn_count = ble_conn_state_for_each_connected(disconnect, NULL);
            NRF_LOG_INFO("Disconnected %d links.", conn_count);
            break;
        }

        case BLE_DFU_EVT_BOOTLOADER_ENTER:
            // YOUR_JOB: Write app-specific unwritten data to FLASH, control finalization of this
            //           by delaying reset by reporting false in app_shutdown_handler
            NRF_LOG_INFO("Device will enter bootloader mode.");
            break;

        case BLE_DFU_EVT_BOOTLOADER_ENTER_FAILED:
            NRF_LOG_ERROR("Request to enter bootloader mode failed asynchroneously.");
            // YOUR_JOB: Take corrective measures to resolve the issue
            //           like calling APP_ERROR_CHECK to reset the device.
            break;

        case BLE_DFU_EVT_RESPONSE_SEND_ERROR:
            NRF_LOG_ERROR("Request to send a response to client failed.");
            // YOUR_JOB: Take corrective measures to resolve the issue
            //           like calling APP_ERROR_CHECK to reset the device.
            APP_ERROR_CHECK(false);
            break;

        default:
            NRF_LOG_ERROR("Unknown event from ble_dfu_buttonless.");
            break;
    }
}

/**@brief Function for initializing services that will be used by the application.
 */
void dfu_services_init(void)
{
    ret_code_t err_code;
    
    ble_dfu_buttonless_init_t dfus_init = {0};  
    // Initialize DFU.
    dfus_init.evt_handler = ble_dfu_evt_handler;
    err_code = ble_dfu_buttonless_init(&dfus_init);
    APP_ERROR_CHECK(err_code);

    NRF_LOG_INFO("dfu_services_init success" );
}



/**@brief Function for the GAP initialization.
 *
 * @details This function sets up all the necessary GAP (Generic Access Profile) parameters of the
 *          device including the device name, appearance, and the preferred connection parameters.
 */
static void gap_params_init(void)
{
    ret_code_t              err_code;
    ble_gap_conn_params_t   gap_conn_params;
    ble_gap_conn_sec_mode_t sec_mode;

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);

    err_code = sd_ble_gap_device_name_set(&sec_mode,
                                          (const uint8_t *)DEVICE_NAME,
                                          strlen(DEVICE_NAME));
    APP_ERROR_CHECK(err_code);

    memset(&gap_conn_params, 0, sizeof(gap_conn_params));

    gap_conn_params.min_conn_interval = MIN_CONN_INTERVAL;
    gap_conn_params.max_conn_interval = MAX_CONN_INTERVAL;
    gap_conn_params.slave_latency     = SLAVE_LATENCY;
    gap_conn_params.conn_sup_timeout  = CONN_SUP_TIMEOUT;

    err_code = sd_ble_gap_ppcp_set(&gap_conn_params);
    APP_ERROR_CHECK(err_code);
}

/**
 * @brief Function for changing the tx power.
 * 
 * @note Supported tx_power values: -40dBm, -20dBm, -16dBm, -12dBm, -8dBm, -4dBm, 0dBm, +3dBm and +4dBm
 * 
 */
void tx_power_set( int8_t tx_power )
{
    ret_code_t err_code = sd_ble_gap_tx_power_set(BLE_GAP_TX_POWER_ROLE_ADV, m_adv_handle, tx_power);
    APP_ERROR_CHECK(err_code);
}


/**
 * @brief 设置Public addresee地址
 *         使用sn号来填充地址
 */
static void ble_address_set(uint8_t addr_type)
{
    ret_code_t    err_code ;
    ble_gap_addr_t gap_addr;

    gap_addr.addr_type = addr_type ;
    memcpy(gap_addr.addr, &g_user_param.sn[2], 6);
    err_code = sd_ble_gap_addr_set(&gap_addr);
    APP_ERROR_CHECK( err_code );
}


/**
 * @brief Function for starting advertising.开始广播
 */
void advertising_start(void)
{
    ret_code_t           err_code;

    if(( false == m_adv_on_off ) && (BLE_CONN_HANDLE_INVALID == m_conn_handle) )
    {
        m_adv_on_off = true ;
        err_code = sd_ble_gap_adv_start(m_adv_handle, APP_BLE_CONN_CFG_TAG);
        APP_ERROR_CHECK(err_code);
        NRF_LOG_INFO("advertising_start");
    }
}

/**
 * @brief 停止广播
 */
void advertising_stop(void)
{
    ret_code_t           err_code;

    if(( true == m_adv_on_off )&&(BLE_CONN_HANDLE_INVALID == m_conn_handle))
    {
        m_adv_on_off = false ;
        err_code = sd_ble_gap_adv_stop(m_adv_handle);
        APP_ERROR_CHECK(err_code);
        NRF_LOG_INFO("advertising_stop");
    }
}


/**
 * @brief 自定义广播报文打包和配置
 */
void advertising_encode_msg_data( adv_enum_e msg_type )
{

    ret_code_t    err_code ;
    ble_gap_adv_data_t adv_data ;
    ble_gap_adv_params_t adv_params ;

    memset(&adv_params, 0, sizeof(ble_gap_adv_params_t));

    static uint8_t advdata[] =                                          // 在家布防广播数据   (广播数据需要static ，不然广播数据有误)
    {                                       
        0x02, 0x01, 0x06,
        0x1a, 0xff,             //长度 类型
        0x4C, 0x00, 0x02, 0x15, //ibeacon标识
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //16
        0x00, 0x01,             //Major Value
        0x00, 0x02,             //Minor Value
        0x01,                   //Measured Power 发射功率，需根据距离计算
    };

    static uint8_t scan_atool_data[31] =
    {
        0x03, 0x03, 0xCE, 0x6C,
        0x1A, 0x16, 0xCE, 0x6C,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //SN
        0,                                              //硬件版本号
        FIRMWARE_VERSION,                               //固件版本号,
        0x00,                                           //设备电量，
        0x00,                                           //BLE功率
        0x00, 0x00, 0x00, 0x00,                         //BLE间隔,float型，小端序
        0x00,                                           //lora 功率
        0x00,                                           //lora sf & adr, 高四位sf,底1位adr
        0x00, 0x00, 0xaa,                               //lora 间隔, 小端序
        0xbb, 0xaa,
    };

    static uint8_t scandata[ ADV_SCANDATA_LEN ];

    memcpy(&advdata[9], g_user_param.ibeacon_uuid, 16);                 //填充uuid
    advdata[25] = g_user_param.ibeacon_major >> 8 ;
    advdata[26] = g_user_param.ibeacon_major & 0xff ;
    advdata[27] = g_user_param.ibeacon_minor >> 8 ;
    advdata[28] = g_user_param.ibeacon_minor & 0xff ;
    advdata[29] = g_user_param.ibeacon_mrssi ;

    adv_data.adv_data.p_data = advdata;
    adv_data.adv_data.len    = sizeof(advdata);


    if( NORMAL_MSG == msg_type )                                        //响应手机端a-tool的app
    {
        memcpy( &scan_atool_data[8] , &g_user_param.sn , 8 );
        adv_data.scan_rsp_data.p_data = scan_atool_data;
        adv_data.scan_rsp_data.len = sizeof(scan_atool_data);      

        adv_params.duration        = 30*100;                            //常规广播30s
    }
    else                                                                //响应声光报扫描的数据
    {
        scandata_encode( msg_type  , scandata );
        adv_data.scan_rsp_data.p_data = scandata;
        adv_data.scan_rsp_data.len = sizeof(scandata);

        adv_params.duration        = g_user_param.ble_int * g_user_param.blefnt / 10 ;  //以自身的参数作用设置广播超时时间 10ms uint
    }

    adv_params.primary_phy     = BLE_GAP_PHY_1MBPS;
    adv_params.properties.type = BLE_GAP_ADV_TYPE_CONNECTABLE_SCANNABLE_UNDIRECTED;
    adv_params.p_peer_addr     = NULL;
    adv_params.filter_policy   = BLE_GAP_ADV_FP_ANY;
    adv_params.interval        = (uint32_t)(g_user_param.ble_int * 1.6);            //以全局变量作为参数，改变广播间隔
    //    adv_params.channel_mask[4] = 0xc0;                                        //关闭广播38，39通道
    err_code = sd_ble_gap_adv_set_configure(&m_adv_handle, &adv_data, &adv_params);
    APP_ERROR_CHECK(err_code);

}


/**
 * 初始化 服务特征
 */
static void ble_service_init(void)
{
    // a-tool 通信服务
    ble_srv_strm_init();

     // 注册 APP 数据读写处理函数
    ble_srv_strm_handler_reg(on_ble_strm_read_handler, on_ble_strm_write_handler);

#if 1               // 是否支持dfu服务
    // ble_dfu 服务初始化
    dfu_services_init();
#endif

}

/**
 * @brief 获取广播持续时间
 */
uint16_t adv_continue_time(void)
{
    return (uint16_t)( g_user_param.ble_int * g_user_param.blefnt );
}


/**
 * ble 事件处理函数
 *
 * @param    p_ble_evt    ble 事件
 */
static void on_ble_evt(ble_evt_t *p_ble_evt)
{
    switch(p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_CONNECTED:
            NRF_LOG_INFO("BLE_GAP_EVT_CONNECTED");
            m_adv_on_off = false ;
            m_conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
            break;

        case BLE_GAP_EVT_DISCONNECTED:
            m_conn_handle = BLE_CONN_HANDLE_INVALID;
            NRF_LOG_INFO("BLE_GAP_EVT_DISCONNECTED");

        case BLE_GAP_EVT_ADV_SET_TERMINATED:                        //广播终止
            m_conn_handle = BLE_CONN_HANDLE_INVALID;
            m_adv_on_off = false ;
            NRF_LOG_INFO("BLE_GAP_EVT_ADV_SET_TERMINATED");
            break;

        default:
            // No implementation needed.
            break;
    }
}

static void ble_evt_dispatch(ble_evt_t const * p_ble_evt, void * p_context)
{                           
    on_ble_evt((ble_evt_t *)p_ble_evt);
    on_ble_srv_strm_evt((ble_evt_t *)p_ble_evt);         //strm服务派发
}   



/**
 *@brief ble相关的初始化 
 */
void ble_init(void)
{
    
    //设置ble地址
    ble_address_set(BLE_GAP_ADDR_TYPE_PUBLIC );   

    //初始化GAP
    gap_params_init();

    //初始化服务特征
    ble_service_init();

    //广播初始化
    advertising_encode_msg_data(NORMAL_MSG);

    //设置发射功率                                
    tx_power_set(g_user_param.ble_txp);                                        

    // Register a handler for BLE events.
    NRF_SDH_BLE_OBSERVER(m_ble_observer, APP_BLE_OBSERVER_PRIO, ble_evt_dispatch, NULL);     
}



