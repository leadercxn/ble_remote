#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "nrf_cli.h"
#include "nrf_cli_types.h"

#include "board_remote001.h"
#include "led_handler.h"

static bool m_module_init = false;
static uint16_t app_led_io[]= { STATUS_LED , };   

static void cmd_gpio_test(nrf_cli_t const * p_cli, size_t argc, char **argv)
{
    uint8_t led_index;
    app_led_para_t app_led_para ={
        .led_pin = app_led_io[0],
        .active_level = 0 ,
        .is_blink = true ,
        .led_on_time = 1,
        .led_off_time = 1 ,
        .count_blink = 2,
    };

    if( false == m_module_init )
    {
        led_handler_init( app_led_io , (sizeof(app_led_io)/sizeof(uint16_t)) , 250 );
        m_module_init = true ;
    }
    led_on_control( app_led_para , &led_index );
    nrf_cli_info(p_cli, "gpio test handler" );
}

NRF_CLI_CMD_REGISTER(gpio, NULL, "gpio test", cmd_gpio_test);

