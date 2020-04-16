/* Copyright (c) 2019 SENSORO Co.,Ltd. All Rights Reserved.
 *
 */
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "nrf_gpio.h"
#include "nrf_drv_gpiote.h"
#include "app_timer.h"
#include "app_button.h"
#include "timer.h"

#include "led_handler.h"
#include "nrf_log.h"

#define LED_LIST_SZ     20


typedef struct                      //该层使用的led参数
{
    app_led_para_t  app_led_para ;
    bool        led_event_alive;    //该led数据是否还有效
    uint32_t    led_currnt_time;    //该led所处时间阶段
    uint32_t    led_current_count_blink;    //该led所处闪烁的次数
} midd_led_para_t;

static midd_led_para_t m_led_list[LED_LIST_SZ] ;        //led控制列表
static uint16_t *mp_pin_group;
static uint16_t m_total_pins;


static uint16_t m_led_timer_period = 100; // 缺省值
APP_TIMER_DEF(m_led_timer); 


/**
 * @brief led开启
 * 
 * @param [out] led_list_index 返回该led在led_list的序号
 * @return 设置结果
 */
uint8_t led_on_control(app_led_para_t app_led_para , uint8_t *led_list_index )
{
    ret_code_t    err_code = NRF_SUCCESS ;

    uint16_t i;
    for( i = 0 ; i < m_total_pins ; i++ )                //遍历输入的io是否已初始化的判断
    {
        if( app_led_para.led_pin == mp_pin_group[i] )
        {
            break;
        }
    }
    if( i == m_total_pins )
    {
        return NRF_ERROR_INVALID_PARAM;
    }

    for( i = 0 ; i < LED_LIST_SZ ; i++ )                 //遍历led控制列表
    {
        if( false == m_led_list[i].led_event_alive   )   //找出空余的列表空间,参数赋值
        {
            memset( &m_led_list[i] , 0 , sizeof(midd_led_para_t) );     //消除旧记录
            m_led_list[i].led_event_alive = true ;
            *led_list_index = i ;                            
            memcpy( &m_led_list[i].app_led_para , &app_led_para ,  sizeof(app_led_para_t) );
            break;
        }
    }
    if( i == LED_LIST_SZ )
    {
        return NRF_ERROR_NO_MEM;
    }

    nrf_gpio_cfg_output(m_led_list[i].app_led_para.led_pin);
    nrf_gpio_pin_write( m_led_list[i].app_led_para.led_pin , m_led_list[i].app_led_para.active_level ); //亮灯

    TIMER_START( m_led_timer, m_led_timer_period );      //打开led定时器
    return err_code;
}

/**
 * @brief led关闭
 */
void led_off_control(uint8_t led_list_index )
{
    m_led_list[led_list_index].led_event_alive = false ;
    nrf_gpio_pin_write( m_led_list[led_list_index].app_led_para.led_pin , (~m_led_list[led_list_index].app_led_para.active_level) ); 
}

/**
 * @brief 布防按键定时器的处理函数      
 */
static void led_timer_timer(void * p_context)
{
    //遍历led控制列表，控制对应的led
    uint8_t i ;
    uint8_t not_alives = 0 ;
    for( i = 0 ; i < LED_LIST_SZ ; i++ )
    {
        not_alives++;
        if( true == m_led_list[i].led_event_alive  )        //找出有效的led事件
        {
            if( m_led_list[i].app_led_para.is_blink  )      //闪烁
            {
                m_led_list[i].led_currnt_time++;            
                if( m_led_list[i].led_currnt_time == (m_led_list[i].app_led_para.led_off_time + m_led_list[i].app_led_para.led_on_time ) )
                {
                    m_led_list[i].led_currnt_time = 0 ;
                    m_led_list[i].led_current_count_blink++;
                }

                if ( m_led_list[i].led_current_count_blink >= m_led_list[i].app_led_para.count_blink )   //闪烁次数用完
                {
                    m_led_list[i].led_event_alive = false ;
                    nrf_gpio_pin_write( m_led_list[i].app_led_para.led_pin , (~m_led_list[i].app_led_para.active_level) );
                    continue;
                }

                if( m_led_list[i].led_currnt_time >= m_led_list[i].app_led_para.led_on_time )        //关
                {
                    nrf_gpio_pin_write( m_led_list[i].app_led_para.led_pin , (~m_led_list[i].app_led_para.active_level) );     
                }
                else                                                                                //开
                {
                    nrf_gpio_pin_write( m_led_list[i].app_led_para.led_pin , m_led_list[i].app_led_para.active_level );
                }
            }
            else                                        //常亮
            {
                nrf_gpio_pin_write( m_led_list[i].app_led_para.led_pin , m_led_list[i].app_led_para.active_level );
            }
            not_alives --;
        }
    }

    if( LED_LIST_SZ  == not_alives )                    //led事件列表全部无效，关闭定时器
    {
        TIMER_STOP(m_led_timer);
        NRF_LOG_INFO("====================> stop the led timer");
    }
}


/**
 * @brief 初始化led io
 */
uint8_t led_handler_init(uint16_t *pin_group , uint16_t total_pins , uint16_t led_timer_period )
{
    ret_code_t    err_code = NRF_SUCCESS ;
    if( 0 == led_timer_period)
    {
        return NRF_ERROR_INVALID_PARAM;
    }
    if( 0 == total_pins)
    {
        return NRF_ERROR_INVALID_PARAM;
    }

    mp_pin_group = pin_group;
    m_total_pins = total_pins;
    m_led_timer_period = led_timer_period ;
    TIMER_CREATE(&m_led_timer, APP_TIMER_MODE_REPEATED, led_timer_timer );  

    return err_code;
}












