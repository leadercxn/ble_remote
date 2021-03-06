/**
 * @brief Blinky Sample Application main file  SDk的官方闪灯例程
 *
 * This file contains the source code for a sample server application using the LED Button service.
 */

#include <stdint.h>
#include <string.h>
#include "app_timer.h"
#include "app_scheduler.h"
#include "nrf_pwr_mgmt.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "board_remote001.h"
#include "timer.h"

#include "battery_handler.h"
#include "fstorage_cfg.h"
#include "fstorage_handler.h"
#include "normal_mode.h"

#include "data_handler.h"
#include "aes_handler.h"

#include "ble_app_handler.h"
#include "button_handler.h"
#include "led_handler.h"

APP_TIMER_DEF(m_battery_timer);                                                 //创建一个电池检测定时器
APP_TIMER_DEF(m_low_power_timer);                                               //创建一个低电压定时器


#define USING_FOR_TEST   0                                                      //成功率测试宏
#if ( USING_FOR_TEST == 1 )
#define  ADV_MAX_CNT        200                                                 //广播次数
#define  ADV_INTERVAL       2000                                              
APP_TIMER_DEF(m_test_timer);                                                    //创建定时广播
static uint16_t m_adv_cnt = 0 ;                                                 //广播计数
uint8_t m_led_on_index = 0 ;
#endif

static uint16_t app_led_io[]= { STATUS_LED , };                                 //led io
#define LED_TIMER_PERIOD            50                                          //控制led timer的周期宽度

#define ADC_TIMER_PERIOD            (10*60*1000)

/**
 * 参数初始化
 */
static void param_init(void)
{
    uint32_t            *p_data         = (uint32_t *) CONFIG_DATA_ADDR;

    p_data = (uint32_t *)ADV_FNT_ADDR;                                      //读取广播次数
    if( 0xFFFFFFFF == *p_data )                                             //第一次使用
    {
        adv_fnt_store();
        NRF_LOG_INFO("First to use , store zero to %08x " , ADV_FNT_ADDR );
    }
    else                                                                    //已经使用过了
    {
        for( g_adv_flash_offset = 1 ; g_adv_flash_offset < 1024 ; g_adv_flash_offset ++ )   //遍历保存次数的flash区
        {
            p_data = p_data + 1 ;
            if( 0xFFFFFFFF == *p_data )                                     //找到未写的区域 
            {
                g_adv_fnt = *(p_data - 1);                                  //记录下上一次掉电前的次数
                break;
            }
        }
    }
    
    p_data = (uint32_t *)CONFIG_BACKUP_ADDR;                                //读取备份区是否存在数据
    if( 0xFFFFFFFF == *p_data )                                             //备份区无数据 
    {
        NRF_LOG_INFO("NO data in user_flash");
        user_param_get_from_fac_oflash();                                   //从原厂区域读出数据
        user_param_store_iflash();                                          //把数据保存到备份区
    }
    else                                                                    //备份区已存在数据
    {
        NRF_LOG_INFO("copy data from user_flash");
        user_param_get_from_user_oflash();                                  //备份区读出数据
    }

    NRF_LOG_INFO("param_init g_adv_fnt = %d ,g_adv_flash_offset = %d " , g_adv_fnt , g_adv_flash_offset );
}



/**
 * @brief led定时器的处理函数      
 */
static void battery_timer_handler(void * p_context)
{
    uint16_t  m_adc_volt_mv = battery_sample();

    NRF_LOG_INFO("m_adc_volt_mv = %d", m_adc_volt_mv );

    if( m_adc_volt_mv < g_user_param.lowpowerlevel )
    {
        NRF_LOG_INFO("low power" );
        TIMER_START( m_low_power_timer, (5000) );               //开启低电压提示
    }
}

/**
 * @brief 低电压提示定时
 */
static void low_power_timer(void * p_context)
{
    ret_code_t err_code = NRF_SUCCESS ;
    uint8_t led_index;
    app_led_para_t app_led_para ={
        .led_pin = app_led_io[0],
        .active_level = 0 ,
        .is_blink = true ,
        .led_on_time = 5,
        .led_off_time = 5 ,
        .count_blink = 2,
    };
    err_code = led_on_control( app_led_para , &led_index );
    APP_ERROR_CHECK(err_code);
}


#if ( USING_FOR_TEST == 1 )
/**
 * @brief TEST定时器的处理函数      
 */
static void test_timer_handler(void * p_context)
{
    ret_code_t err_code = NRF_SUCCESS ;
    uint8_t led_index;
    app_led_para_t app_led_para ={
        .led_pin = app_led_io[0],
        .active_level = 0 ,
        .is_blink = true ,
        .led_on_time = 1,
        .led_off_time = 1 ,
        .count_blink = 2,
    };

    m_adv_cnt++;
#if 1
    if( m_adv_cnt > ADV_MAX_CNT )
    {
        TIMER_STOP(m_test_timer);
        app_led_para.is_blink = 0 ;
        err_code = led_on_control( app_led_para , &led_index );
        m_led_on_index = led_index ; 
        APP_ERROR_CHECK(err_code);
        return ;
    }
#endif 
    g_adv_fnt++ ;    

    advertising_encode_msg_data( GUARD_SET_MSG );      //打包广播数据
    advertising_start();                                //开启广播
    NRF_LOG_INFO("m_adv_cnt = %d" , m_adv_cnt );

    err_code = led_on_control( app_led_para , &led_index );
    APP_ERROR_CHECK(err_code);
}
#endif


/**@brief Function for the Timer initialization.
 *
 * @details Initializes the timer module.
 */
static void timers_init(void)
{
    TIMER_CREATE(&m_battery_timer,      APP_TIMER_MODE_REPEATED,    battery_timer_handler   );          //创建电池检测定时器
    TIMER_CREATE(&m_low_power_timer,    APP_TIMER_MODE_REPEATED,    low_power_timer         );          //低电压闪烁定时器
    TIMER_START( m_battery_timer, ADC_TIMER_PERIOD );   //开启电池检测定时器

#if ( USING_FOR_TEST == 1 )
    TIMER_CREATE(&m_test_timer, APP_TIMER_MODE_REPEATED, test_timer_handler);     //创建自动测试定时器
#endif

}

/**
 * 按键对应处理事件
 */
void button_event_handler(uint8_t event)
{
    NRF_LOG_INFO("button event = %s " , button_event_log_str(event) );
    adv_enum_e adv_msg_type;
    ret_code_t err_code = NRF_SUCCESS ;

    uint8_t led_index;
    app_led_para_t app_led_para ={
        .led_pin = app_led_io[0],
        .active_level = 0 ,
        .is_blink = true ,
        .led_on_time = 1,
        .led_off_time = 1 ,
        .count_blink = 2,
    };

    advertising_stop();                               //停止广播,

    switch(event)
    {
        case BUTTON_EVENT_GUARD_HOME:
                adv_msg_type = GUARD_HOME_MSG ;
            break;
        case BUTTON_EVENT_ALARM_SET:
                adv_msg_type = ALARM_SET_MSG ;
            break;
        case BUTTON_EVENT_GUARD_SET:
                adv_msg_type = GUARD_SET_MSG ;
#if ( USING_FOR_TEST == 1 )
    TIMER_START(m_test_timer , ADV_INTERVAL );
    return ;
#endif
            break;
        case BUTTON_EVENT_GUARD_CANCEL:
                adv_msg_type = GUARD_CANCEL_MSG ;
#if ( USING_FOR_TEST == 1 )
    TIMER_STOP(m_test_timer);
    m_adv_cnt = 0 ;
    led_off_control(m_led_on_index);
    APP_ERROR_CHECK(err_code);
    return ;
#endif
            break;
        case BUTTON_EVENT_PAIR:
                adv_msg_type = PAIR_MSG ;
            break;
        default:
            adv_msg_type = NORMAL_MSG ;
            break;
    }

    NRF_LOG_INFO("adv_msg_type = 0x%02x" , adv_msg_type );

    advertising_encode_msg_data( adv_msg_type );        //打包广播数据
    advertising_start();                                //开启广播

    err_code = led_on_control( app_led_para , &led_index );
    APP_ERROR_CHECK(err_code);

    g_adv_fnt++;                                        //广播计数累计
    adv_fnt_store();
}

/**
 * @brief 按键处理层初始化
 */
static void button_handler_init(void)
{
    buttons_init();
    button_event_handler_register(button_event_handler);
}

/**@brief Function for handling the idle state (main loop).
 *
 * @details If there is no pending log operation, then sleep until next the next event occurs.
 */
void idle_state_handle(void)
{
    if (NRF_LOG_PROCESS() == false)
    {
        nrf_pwr_mgmt_run();
    }
}


/**@brief Function for application normal_mode main entry.
 */
void normal_mode(void)
{
    ret_code_t err_code;
    
    //参数初始化
    param_init();

    //定时器初始化
    timers_init();

    //led初始化
    err_code = led_handler_init( app_led_io , (sizeof(app_led_io)/sizeof(uint16_t)) , LED_TIMER_PERIOD );
    APP_ERROR_CHECK(err_code);

    //按键初始化
    button_handler_init();

    //rng随机数初始化
    rng_init();                                     

    //ble
    ble_init();   

    //电池检测初始化                                          
    battery_init();                                         


    NRF_LOG_INFO("GIT_VAR = %s " , GIT_VAR);
    NRF_LOG_INFO("DATE_VAR = %s " , DATE_VAR);
    for (;;)
    {
        app_sched_execute();
        idle_state_handle();
    }

}


/**
 * @}
 */
