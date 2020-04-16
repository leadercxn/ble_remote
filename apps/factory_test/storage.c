#include <stdint.h>
#include <string.h>
#include "nordic_common.h"
#include "nrf.h"
#include "nrf_sdm.h"
#include "app_error.h"
#include "nrf_fstorage.h"
#include "nrf_fstorage_sd.h"
#include "nrf_sdh.h"
#include "fds.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#include "faccfg.pb.h"

#include "crc16.h"
#include "pb_decode.h"
#include "storage.h"



storage_app_param_t     app_param;                      /* configuration data. 根据不同的产品的配置参数*/
storage_user_param_t    user_param;                     //user定义的数据结构


static volatile bool m_fds_ready      = false;      /**< Flag used to indicate that FDS initialization is finished. */
static volatile bool m_pending_write  = false;      /**< Flag used to preserve write request during Garbage Collector activity. */
static volatile bool m_pending_update = false;      /**< Flag used to preserve update request during Garbage Collector activity. */

static uint32_t        m_pending_msg_size   = 0;    /**< Pending write/update request data size. */
static uint8_t const * m_p_pending_msg_buff = NULL; /**< Pending write/update request data pointer. */

static fds_record_desc_t  m_record_desc;            //Record descriptor structure  用来操作record（有点类似linux文件的文件描述符）
static fds_record_t       m_record;                 //Record description used for writes. 用来写

/* Array to map FDS events to strings. */
static char const * fds_evt_str[] =
{
    "FDS_EVT_INIT",
    "FDS_EVT_WRITE",
    "FDS_EVT_UPDATE",
    "FDS_EVT_DEL_RECORD",
    "FDS_EVT_DEL_FILE",
    "FDS_EVT_GC",
};



static void fstorage_evt_handler(nrf_fstorage_evt_t * p_evt);

NRF_FSTORAGE_DEF(nrf_fstorage_t fs_storage) =
{
    /* Set a handler for fstorage events. */
    .evt_handler = fstorage_evt_handler,

    /* These below are the boundaries of the flash space assigned to this instance of fstorage.
     * You must set these manually, even at runtime, before nrf_fstorage_init() is called.
     * The function nrf5_flash_end_addr_get() can be used to retrieve the last address on the
     * last page of flash available to write data. */
    #if 0
    .start_addr = 0x2A000,          //168K
    .end_addr   = 0x2FFFF,          //192k
    #endif
    .start_addr = 0x29000,          //164K
    .end_addr   = 0x29FFF,          //168k
};

const char *fds_err_str(ret_code_t ret)
{
    /* Array to map FDS return values to strings. */
    static char const * err_str[] =
    {
        "FDS_ERR_OPERATION_TIMEOUT",
        "FDS_ERR_NOT_INITIALIZED",
        "FDS_ERR_UNALIGNED_ADDR",
        "FDS_ERR_INVALID_ARG",
        "FDS_ERR_NULL_ARG",
        "FDS_ERR_NO_OPEN_RECORDS",
        "FDS_ERR_NO_SPACE_IN_FLASH",
        "FDS_ERR_NO_SPACE_IN_QUEUES",
        "FDS_ERR_RECORD_TOO_LARGE",
        "FDS_ERR_NOT_FOUND",
        "FDS_ERR_NO_PAGES",
        "FDS_ERR_USER_LIMIT_REACHED",
        "FDS_ERR_CRC_CHECK_FAILED",
        "FDS_ERR_BUSY",
        "FDS_ERR_INTERNAL",
    };

    return err_str[ret - NRF_ERROR_FDS_ERR_BASE];
}

static void storage_app_file_prepare(uint8_t const * p_buff, uint32_t size)
{
    m_record.file_id = CONFIG_APP_FILE;
    m_record.key = CONFIG_APP_PARAM_KEY;
    m_record.data.p_data = p_buff;
    m_record.data.length_words = BYTES_TO_WORDS(size);
}

/**
 * @brief 更新record文件 
 */
static ret_code_t storage_file_create(uint8_t const * p_buff, uint32_t size)
{
    ret_code_t err_code;
    
    storage_app_file_prepare(p_buff, size);
    
    err_code = fds_record_write(&m_record_desc, &m_record);
    if(err_code == FDS_ERR_NO_SPACE_IN_FLASH)
    {
        m_pending_write      = true;
        m_pending_msg_size   = size;
        m_p_pending_msg_buff = p_buff;
        NRF_LOG_INFO("FDS has no free space left, Garbage Collector triggered!");
        err_code = fds_gc();
    }
    
    return err_code;
}

/**
 * @brief 更新record文件 
 */
ret_code_t storage_file_update(uint8_t const * p_buff, uint32_t size)
{
    ret_code_t err_code;
    
    storage_app_file_prepare(p_buff, size);
    
    err_code = fds_record_update(&m_record_desc, &m_record);
    if(err_code == FDS_ERR_NO_SPACE_IN_FLASH)
    {
        // If there is no space, preserve update request and call Garbage Collector.
        m_pending_update     = true;
        m_pending_msg_size   = size;
        m_p_pending_msg_buff = p_buff;
        NRF_LOG_INFO("FDS has no space left, Garbage Collector triggered!");
        err_code = fds_gc();
    }
    
    return err_code;
}

/**
 * @brief 从flash区载入相关的数据(上电的时候读取flash区里面的配置数据)
 * 
 *  @param[out]  p_buff  record data 
 */
ret_code_t storage_file_load(uint8_t * p_buff)          
{
    ret_code_t err_code;
    fds_find_token_t tok = {0};
    fds_flash_record_t config = {0};            // 用来装载open file record里面的数据
    
    err_code = fds_record_find(CONFIG_APP_FILE, CONFIG_APP_PARAM_KEY, &m_record_desc, &tok);    //根据file_id、key_id查找fds区里面的数据
    
    if(err_code == NRF_SUCCESS)
    {
        NRF_LOG_INFO("Found storage file record.");
        
        // Open record for read.
        err_code = fds_record_open(&m_record_desc, &config);            //record描述符 、 record返回的数据结构体(config)
        APP_ERROR_CHECK(err_code);
        
        // Access the record through the flash_record structure.
        memcpy(p_buff,                                                  //拷出flash区里面的数据到p_buff
               config.p_data,
               config.p_header->length_words * sizeof(uint32_t));
        
        // Print file length and raw message data.
        NRF_LOG_INFO("Storage file data length: %u bytes.",
                      config.p_header->length_words * sizeof(uint32_t));

        NRF_LOG_HEXDUMP_INFO(p_buff, config.p_header->length_words * sizeof(uint32_t));     //打印数据

        // Close the record when done.
        err_code = fds_record_close(&m_record_desc);                    //关闭record文件
    }
    else if(err_code == FDS_ERR_NOT_FOUND)
    {
        NRF_LOG_INFO("Storage file record not found.");
    }
    
    return err_code;
}

/**
 * @brief 根据file_id、key_id查找record文件
 */
bool storage_file_find(void)
{
    ret_code_t err_code;
    fds_find_token_t tok = {0};
    
    err_code = fds_record_find(CONFIG_APP_FILE, CONFIG_APP_PARAM_KEY, &m_record_desc, &tok);
    
    return (err_code == NRF_SUCCESS);
}

/**
 * @brief 在flash区查找（自定义）配置参数
 */
bool config_find(void)
{

#if 0
    storage_app_param_t tmp;
#endif

#if 0
    storage_user_param_t tmp; 

    if(storage_file_load((uint8_t *)&tmp) == NRF_SUCCESS)
    {
        #if 0
        uint16_t crc = 0;
        
        crc = crc16_compute((uint8_t *)&tmp.crc + 2, sizeof(storage_app_param_t) - 4, NULL);
        
        return ((tmp.crc == crc) && (tmp.magic_byte == MAGIC_BYTE));
        #endif
        
        #if 0

        NRF_LOG_INFO(" tmp.magic_byte = 0x%04x " , tmp.magic_byte);
        NRF_LOG_INFO(" tmp.hw_ver = 0x%04x " , tmp.hw_ver);
        NRF_LOG_INFO(" tmp.fw_ver = 0x%04x " , tmp.fw_ver);
        NRF_LOG_INFO(" tmp.crc = 0x%04x " , tmp.crc);
        NRF_LOG_INFO(" tmp.ble_int = 0x%04x " , tmp.ble_int);
        NRF_LOG_INFO(" tmp.ble_txp = 0x%04x " , tmp.ble_txp);

        NRF_LOG_INFO(" tmp.sn : ");
        NRF_LOG_HEXDUMP_INFO(tmp.sn, sizeof(tmp.sn));     //打印数据

        #endif

        return ((tmp.crc == 0xaa) && (tmp.magic_byte == MAGIC_BYTE));
    }
#endif
    if(storage_file_load((uint8_t *)&user_param) == NRF_SUCCESS)
    {

        return ((user_param.crc == 0xaa) && (user_param.magic_byte == MAGIC_BYTE));
    }
    else
    {
        return false;
    }
}

/**
 * @brief 更新配置参数
 */
void storage_app_cfg_update(void)
{
    if(storage_file_find())                                                         //查找record文件
    {
#if 0
        storage_file_update((uint8_t *)&app_param, sizeof(storage_app_param_t));    //若已找到record文件，更新record文件的内容
#endif
        NRF_LOG_INFO("Perform storage_file_update ");
        storage_file_update((uint8_t *)&user_param, sizeof(storage_user_param_t));    //若已找到record文件，更新record文件的内容
    }
    else
    {
#if 0
        storage_file_create((uint8_t *)&app_param, sizeof(storage_app_param_t));    //若没找到record文件，创建record文件
#endif
        NRF_LOG_INFO("Perform storage_file_create ");
        storage_file_create((uint8_t *)&user_param, sizeof(storage_user_param_t));    //若没找到record文件，创建record文件(第一次初始化使用会创建record文件)
    }
}


/**
 * @brief 
 */
static void config_from_flash(void)
{

#if 0
    // 配置工具把配置数据写到地址 FAC_SWD_CFG_DATA_ADDR
    // 配置数据格式为 'length + data'
    // length 2 字节, 小端序
    // data 为 proto 打包后的数据

    uint32_t *p_data;
    uint8_t const *p_cfg_data;
    uint16_t cfg_len = 0;
    bool status;
    
    p_data = (uint32_t *)nrf_fstorage_rmap(&fs_storage, FAC_SWD_CFG_DATA_ADDR);
    p_cfg_data = nrf_fstorage_rmap(&fs_storage, FAC_SWD_CFG_DATA_ADDR);
    
    if(*p_data == 0xffffffff)                                           //未发现配置数据
    {
        NRF_LOG_INFO("No configuration found.");
        return;
    }
    
    cfg_len = uint16_decode(p_cfg_data);
    NRF_LOG_INFO("Configuration len: %d", cfg_len);
    
    p_cfg_data = p_cfg_data + 2;
    
    fac_Config fac_cfg = fac_Config_init_zero;
    pb_istream_t stream = pb_istream_from_buffer(p_cfg_data, cfg_len);  //从flash区里创建一个输入数据流
    status = pb_decode(&stream, fac_Config_fields, &fac_cfg);           //从数据流中读出数据放入 fac_cfg 的buff里面
    
    if(!status)
    {
        NRF_LOG_INFO("PB DECODE FAILED");
        return;
    }
    
    NRF_LOG_INFO("Start configure from flash");
    
    app_param.magic_byte = MAGIC_BYTE;
    app_param.hw_ver = uint16_big_decode(fac_cfg.appHardwareversion.bytes);
    app_param.fw_ver = FW_VERSION;
    
    memcpy(app_param.sn, fac_cfg.appSn.bytes, fac_cfg.appSn.size);
    memcpy(app_param.tok, fac_cfg.appToken.bytes, fac_cfg.appToken.size);
    
    memcpy(app_param.ble_passwd, fac_cfg.appPassword.bytes, fac_cfg.appPassword.size);
    memcpy(app_param.secure_key, fac_cfg.appSecureKey.bytes, fac_cfg.appSecureKey.size);
    
    app_param.ble_txp = fac_cfg.appBleTxPower;
    app_param.ble_int = fac_cfg.appBleInterval;
    
    app_param.report_interval = fac_cfg.appInterval;
    
    memcpy(app_param.ibeacon_uuid, fac_cfg.appUUID.bytes, fac_cfg.appUUID.size);
    app_param.ibeacon_major = uint16_big_decode(&app_param.sn[4]);
    app_param.ibeacon_minor = uint16_big_decode(&app_param.sn[6]);
    app_param.ibeacon_rssi = (int8_t)fac_cfg.appRSSI;
    
    app_param.alarm_shield_time = fac_cfg.appAlarmShieldTime;
    app_param.insulate_switch = fac_cfg.appInsulateSwitch;
    app_param.low_battery_beep = fac_cfg.appLowBatteryBeep;
    app_param.led_switch = fac_cfg.appLedStatus;
    app_param.demo_mode = fac_cfg.appDemoMode;
    app_param.cds_switch = fac_cfg.appCdsSwitch;
    app_param.warning_switch = fac_cfg.appWarningSwtich;
    app_param.human_detection_switch = fac_cfg.appHumanDetectionSwitch;
    app_param.human_detection_sync = fac_cfg.appHumanDetectionSync;
    app_param.night_light = fac_cfg.appNightLight;
    app_param.machine_tested = false;
#endif

#if 1                                                                    //仿flash赋值数据
    static uint8_t tmp_sn[8] = { 0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08 };

    user_param.magic_byte = MAGIC_BYTE ;
    user_param.hw_ver = 0x01 ;
    user_param.fw_ver = 0x10 ;
    user_param.crc    = 0xaa ;
    user_param.ble_txp = 1   ;
    user_param.ble_int = 32  ;
    
    memcpy( user_param.sn , tmp_sn , sizeof(tmp_sn) );
#endif

    storage_app_cfg_update();                                           //app的数据更新到flash区里面                                   
}

static void fstorage_evt_handler(nrf_fstorage_evt_t * p_evt)
{
    
}

static void fds_evt_handler(fds_evt_t const * p_evt)
{
    ret_code_t err_code;
        
    if( NRF_SUCCESS == p_evt->result )
    {
        NRF_LOG_INFO("Event: %s received (NRF_SUCCESS)",
                      fds_evt_str[p_evt->id]);
    }
    else
    {
        NRF_LOG_INFO("Event: %s received (%s)",
                      fds_evt_str[p_evt->id],
                      fds_err_str(p_evt->result));
    }
    

    switch(p_evt->id)
    {
        case FDS_EVT_INIT:                      //fds初始化完成
            APP_ERROR_CHECK(p_evt->result);
            m_fds_ready = true;
            break;
            
        case FDS_EVT_UPDATE:
            APP_ERROR_CHECK(p_evt->result);
            NRF_LOG_INFO("FDS update success");
            break;
            
        case FDS_EVT_WRITE:
            APP_ERROR_CHECK(p_evt->result);
            NRF_LOG_INFO("FDS write success");
            break;
            
        case FDS_EVT_GC:
            APP_ERROR_CHECK(p_evt->result);
            NRF_LOG_INFO("Garbage Collector activity finished.");
            
            if(m_pending_write)
            {
                NRF_LOG_DEBUG("Write pending msg.", p_evt->id, p_evt->result);
                m_pending_write = false;
                err_code        = storage_file_create(m_p_pending_msg_buff, m_pending_msg_size);
                APP_ERROR_CHECK(err_code);
            }
            else if(m_pending_update)
            {
                NRF_LOG_DEBUG("Update pending msg.", p_evt->id, p_evt->result);
                m_pending_update = false;
                err_code         = storage_file_create(m_p_pending_msg_buff, m_pending_msg_size);
                APP_ERROR_CHECK(err_code);
            }
            break;
            
        default:
            break;
    }
}

void storage_init(void)
{
    ret_code_t rc;
    
    nrf_fstorage_api_t * p_fs_api;
    p_fs_api = &nrf_fstorage_sd;                            //初始化一个通过sd协议栈写入flash的api接口结构
    
    rc = nrf_fstorage_init(&fs_storage, p_fs_api, NULL);    //初始化一个fs_storage,和传递fs_api接口函数
    APP_ERROR_CHECK(rc);
    
    /* Register first to receive an event when initialization is complete. */
    rc = fds_register(fds_evt_handler);                     //注册fds处理事件函数
    APP_ERROR_CHECK(rc);
    
    rc = fds_init();                                        //fds初始化
    APP_ERROR_CHECK(rc);

    /* Wait for fds to initialize. */
    while(!m_fds_ready);                                    //等待fds初始化完成，
    
    NRF_LOG_INFO("Reading flash usage statistics...");

    fds_stat_t stat = {0};

    rc = fds_stat(&stat);                                   //获取fds状态
    APP_ERROR_CHECK(rc);

    NRF_LOG_INFO("Found %d valid records.", stat.valid_records);
    NRF_LOG_INFO("Found %d dirty records (ready to be garbage collected).", stat.dirty_records);
    NRF_LOG_INFO("Word used %d", stat.words_used);
    NRF_LOG_INFO("Word reserved %d", stat.words_reserved);
    NRF_LOG_INFO("Freeable words %d", stat.freeable_words);
    
    if(!config_find())                                      //查找(自定义)配置的参数
    {
        NRF_LOG_INFO("Can't find the config , that will creat or  update a record file ");
        config_from_flash();                                //假如加载(找)不到，则重新更新flash区或创建record文件
    }
}
