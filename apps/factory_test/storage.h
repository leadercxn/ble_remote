#ifndef STORAGE_H__
#define STORAGE_H__

/* File ID and Key used for the configuration record. */

#define CONFIG_APP_FILE             (0x0001)
#define CONFIG_APP_PARAM_KEY        (0x0001)
#define CONFIG_FAC_APP_PARAM_KEY    (0x0002)

#define FAC_SWD_CFG_DATA_ADDR       0xf3000
#define FAC_SWD_CFG_FW_VER_ADDR     0xf3400

#define MAGIC_BYTE      0x01
#define FW_VERSION      0x0100

/* App structure to save in flash. */

typedef struct
{
    uint16_t magic_byte;
    uint16_t crc;
    uint16_t hw_ver;
    uint16_t fw_ver;
    uint8_t sn[8];
    uint8_t tok[16];
    
    int8_t ble_txp;
    float ble_int;
    
    uint8_t ble_passwd[16];
    uint8_t secure_key[16];
    
    uint32_t report_interval;
    
    uint8_t  ibeacon_uuid[16];
    uint16_t ibeacon_major;
    uint16_t ibeacon_minor;
    int8_t   ibeacon_rssi;
    
    uint8_t deploy;
    uint8_t insurance_activate;
    uint8_t demo_mode;
    uint8_t low_battery_beep;
    uint8_t led_switch;
    uint8_t cds_switch;
    uint8_t warning_switch;
    uint8_t human_detection_switch;
    uint8_t human_detection_sync;
    uint8_t night_light;
    uint16_t alarm_shield_time;
    uint8_t insulate_switch;
    bool machine_tested;
} storage_app_param_t;



typedef struct 
{
    uint16_t magic_byte;
    uint16_t crc;
    uint16_t hw_ver;
    uint16_t fw_ver;
    uint8_t sn[8];

    int8_t ble_txp;                     
//    float  ble_int;                   //以前传感器的数据类型
    uint16_t ble_int;

} storage_user_param_t;


extern storage_app_param_t     app_param;                      /* configuration data. 根据不同的产品的配置参数*/
extern storage_user_param_t    user_param;                     //user定义的数据结构

void storage_init(void);

void storage_app_cfg_update(void);


extern storage_app_param_t app_param;

#endif

