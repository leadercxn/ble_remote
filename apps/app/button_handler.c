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

#include "board_remote001.h"
#include "button_handler.h"

#define BUTTON_DETECTION_DELAY          APP_TIMER_TICKS(50)  /**< Delay from a GPIOTE event until a button is reported as pushed (in number of timer ticks). */
#define BUTTON_LONG_PUSH_TIME_MS        5000                 //长按计时周期

APP_TIMER_DEF(m_button_guard_set_timer);                     //创建一个布防按键定时器
APP_TIMER_DEF(m_button_guard_cancel_timer);                  //创建一个撤防按键定时器
static button_event_handler_t m_handler = NULL;              //定义了一个函数指针void (*button_event_handler_t) (uint8_t event);传参进去的是事件枚举
static uint16_t m_button_guard_set_push_fnt = 0 ; 
static uint16_t m_button_guard_cancel_push_fnt = 0 ; 

/**
 * @brief button事件
 */
char *button_event_log_str(uint8_t evt_type)
{
    if(evt_type > BUTTON_EVENT_IBEACON)
    {
        return "NULL";
    }
    static char *event_str[]=
    {
        "BUTTON_EVENT_GUARD_HOME",
        "BUTTON_EVENT_ALARM_SET",
        "BUTTON_EVENT_GUARD_SET",
        "BUTTON_EVENT_GUARD_CANCEL",
        "BUTTON_EVENT_PAIR",
        "BUTTON_EVENT_IBEACON",
    };
    return event_str[evt_type];
}


/**
 * @brief 布防按键定时器的处理函数      
 */
static void guard_set_timer_handler(void * p_context)
{
    m_button_guard_set_push_fnt++;
    m_handler(BUTTON_EVENT_PAIR);
}

/**
 * @brief 撤防按键定时器的处理函数      
 */
static void guard_cancel_timer_handler(void * p_context)
{
    m_button_guard_cancel_push_fnt++;
    m_handler(BUTTON_EVENT_IBEACON);
}

/**
 * @brief 初始化按键用到的定时器
 */
static void button_timer_init(void)
{
    TIMER_CREATE(&m_button_guard_set_timer, APP_TIMER_MODE_SINGLE_SHOT, guard_set_timer_handler );         
    TIMER_CREATE(&m_button_guard_cancel_timer, APP_TIMER_MODE_SINGLE_SHOT, guard_cancel_timer_handler );    
}


/**
 * 根据按键的动作，把事件号传递给上层
 * 
 */
static void button_event_handler(uint8_t pin_no, uint8_t button_action)
{
    if (m_handler == NULL)
    {
        return;
    }

    switch (pin_no)
    {
        case GUARD_HOME_BUTTON:                         //在家布防
            if (APP_BUTTON_PUSH == button_action)       //按下
            {
                m_handler(BUTTON_EVENT_GUARD_HOME);
            }
            break;
        case ALARM_SET_BUTTON:                          //一键报警
            if (APP_BUTTON_PUSH == button_action)       //按下
            {
                m_handler(BUTTON_EVENT_ALARM_SET);
            }
            break;
        case GUARD_SET_BUTTON:                          //布防按键
            if (APP_BUTTON_PUSH == button_action)       //按下
            {
                TIMER_START( m_button_guard_set_timer, BUTTON_LONG_PUSH_TIME_MS ); 
                m_button_guard_set_push_fnt = 0 ;
            }
            else
            {
                TIMER_STOP( m_button_guard_set_timer );
                if( 0 == m_button_guard_set_push_fnt )
                {
                    m_handler(BUTTON_EVENT_GUARD_SET);
                }
            }
            break;
        case GUARD_CANCEL_BUTTON:                       //撤防按键
            if (APP_BUTTON_PUSH == button_action)       //按下
            {
                TIMER_START( m_button_guard_cancel_timer, BUTTON_LONG_PUSH_TIME_MS ); 
                m_button_guard_cancel_push_fnt = 0 ;
            }
            else
            {
                TIMER_STOP( m_button_guard_cancel_timer );
                if( 0 == m_button_guard_cancel_push_fnt )
                {
                    m_handler(BUTTON_EVENT_GUARD_CANCEL);
                }
            }
            break;
        default:
            break;
    }
}




/**
 * @brief Function for initializing the button handler module.
 * 按键初始化
 */
void buttons_init(void)
{
    ret_code_t err_code;
    
    //The array must be static because a pointer to it will be saved in the button handler module.
    static app_button_cfg_t buttons[] =
    {
        { ALARM_SET_BUTTON, APP_BUTTON_ACTIVE_LOW, NRF_GPIO_PIN_NOPULL, button_event_handler},
        {  GUARD_HOME_BUTTON, APP_BUTTON_ACTIVE_LOW, NRF_GPIO_PIN_NOPULL, button_event_handler},
        {  GUARD_SET_BUTTON, APP_BUTTON_ACTIVE_LOW, NRF_GPIO_PIN_NOPULL, button_event_handler},
        { GUARD_CANCEL_BUTTON, APP_BUTTON_ACTIVE_LOW, NRF_GPIO_PIN_NOPULL, button_event_handler},
    };

    err_code = app_button_init(buttons, ARRAY_SIZE(buttons),BUTTON_DETECTION_DELAY);
    APP_ERROR_CHECK(err_code);

    err_code = app_button_enable();                                                             //使能按键事件
    APP_ERROR_CHECK(err_code);

    button_timer_init();            //初始化按键用到的定时器
}



/**
 * 把上层事件处理函数传递到该层使用
 * 
 */
void button_event_handler_register(button_event_handler_t handler)
{    
    m_handler = handler;
}
