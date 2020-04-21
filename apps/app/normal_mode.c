/* Copyright (c) 2020 SENSORO Co.,Ltd. All Rights Reserved.
 *
 */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "app_error.h"
#include "app_timer.h"
#include "app_scheduler.h"

#include "nrf_pwr_mgmt.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#include "ble_handler.h"
#include "led_handler.h"
#include "button_handler.h"
#include "remote_cmd.h"

#include "normal_mode.h"

/**@brief Function for the Timer initialization.
 *
 * @details Initializes the timer module. This creates and starts application timers.
 */
static void timers_init(void)
{
    // Initialize timer module.
    ret_code_t err_code = app_timer_init();
    APP_ERROR_CHECK(err_code);
}

/**@brief Function for starting timers.
 */
static void timers_start(void)
{

}
/**@brief Function for initializing power management.
 */
static void power_management_init(void)
{
    ret_code_t err_code;
    err_code = nrf_pwr_mgmt_init();
    APP_ERROR_CHECK(err_code);
}

/**@brief Function for handling the idle state (main loop).
 *
 * @details If there is no pending log operation, then sleep until next the next event occurs.
 */
static void idle_state_handle(void)
{
    if(NRF_LOG_PROCESS() == false)
    {
        nrf_pwr_mgmt_run();
    }
}

static void button_event_handler(button_event_t event)
{
    uint8_t cmd_data[31] = {0};
    uint16_t size = 0;

    static uint16_t cnt = 0;

    switch(event)
    {
        case BUTTON_EVENT_ARMING_PUSH:
            NRF_LOG_INFO("BUTTON_EVENT_ARMING_PUSH");

            remote_cmd_data_get(0x01, cnt++, cmd_data, &size);
            ble_advertising_scan_rsp_update(cmd_data, size);
            break;

        case BUTTON_EVENT_DISARMING_PUSH:
            NRF_LOG_INFO("BUTTON_EVENT_DISARMING_PUSH");

            remote_cmd_data_get(0x02, cnt++, cmd_data, &size);
            ble_advertising_scan_rsp_update(cmd_data, size);
            break;

        case BUTTON_EVENT_HOME_ARMING_PUSH:
            NRF_LOG_INFO("BUTTON_EVENT_HOME_ARMING_PUSH");

            remote_cmd_data_get(0x03, cnt++, cmd_data, &size);
            ble_advertising_scan_rsp_update(cmd_data, size);
            break;

        case BUTTON_EVENT_ALARM_PUSH:
            NRF_LOG_INFO("BUTTON_EVENT_ALARM_PUSH");

            remote_cmd_data_get(0x04, cnt++, cmd_data, &size);
            ble_advertising_scan_rsp_update(cmd_data, size);
            break;

        default:
            break;
    }
}

void normal_mode(void)
{
    NRF_LOG_INFO("SENSORO %s %s, normal mode", PROJECT_NAME, APP_VERSION_HASH);

    timers_init();
    power_management_init();

    led_init();

    button_init();
    button_event_handler_register(button_event_handler);

    remote_cmd_init();

    ble_init();

    timers_start();

    led_on_start();

    NRF_LOG_INFO("loop");
    for(;;)
    {
        app_sched_execute();
        idle_state_handle();
    }
}
