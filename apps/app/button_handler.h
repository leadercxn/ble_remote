/* Copyright (c) 2019 SENSORO Co.,Ltd. All Rights Reserved.
 *
 */

#ifndef __BUTTON_HANDLER_H__
#define __BUTTON_HANDLER_H__

//按键的事件枚举
enum
{
    BUTTON_EVENT_GUARD_HOME,    //在家布防
    BUTTON_EVENT_ALARM_SET,     //一键报警
    BUTTON_EVENT_GUARD_SET,     //布防
    BUTTON_EVENT_GUARD_CANCEL,  //撤防
    BUTTON_EVENT_PAIR,          //配对
    BUTTON_EVENT_IBEACON,       //normal-ibeacon
};


/**
 * [void  description]
 *
 * @param     event    [description]
 *
 * @return             [description]
 */
typedef void (*button_event_handler_t) (uint8_t event);

/**
 * @brief Function for initializing the button handler module.
 * 按键初始化
 */
void buttons_init(void);

/**
 * 把上层事件处理函数传递到该层使用
 * 
 */
void button_event_handler_register(button_event_handler_t handler);

/**
 * @brief button事件
 */
char *button_event_log_str(uint8_t evt_type);


#endif
