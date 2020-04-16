

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "nrf_cli.h"
#include "nrf_cli_types.h"

#include "battery_handler.h"

static bool m_module_init = false;

static void cmd_battery(nrf_cli_t const * p_cli, size_t argc, char **argv)
{

    UNUSED_PARAMETER(argc);

    UNUSED_PARAMETER(argv);

}

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


NRF_CLI_CREATE_STATIC_SUBCMD_SET(m_battery_cmd)

{

    NRF_CLI_CMD(vol,   NULL, "get the battery voltage", cmd_battery_test),

    NRF_CLI_SUBCMD_SET_END

};

NRF_CLI_CMD_REGISTER(battery, &m_battery_cmd, "battery", cmd_battery);




/**用法
 * demo:
 * 
 *  >>battery vol
 *  battery voltage = 3330 mv
 */



