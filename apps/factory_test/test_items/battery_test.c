

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "nrf_cli.h"
#include "nrf_cli_types.h"

#include "battery_handler.h"

static bool m_module_init = false;


static void cmd_battery_test(nrf_cli_t const * p_cli, size_t argc, char **argv)
{
    uint16_t adc_volt_mv = 0 ;

    if( false == m_module_init )
    {
        battery_init();                                         //电池检测初始化
        m_module_init = true ;
    }

    adc_volt_mv =  battery_sample();

    nrf_cli_info(p_cli, "battery voltage = %d mv" , adc_volt_mv );
}

NRF_CLI_CMD_REGISTER(battery, NULL, "get the battery  volt", cmd_battery_test);


