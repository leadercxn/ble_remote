

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "nrf_cli.h"
#include "nrf_cli_types.h"

//#define SUBG_CMD


#ifdef SUBG_CMD   
static void cmd_usage(nrf_cli_t const * p_cli, size_t argc, char **argv)
{
    UNUSED_PARAMETER(argc);
    UNUSED_PARAMETER(argv);
    //nrf_cli_info(p_cli, "uage cmd" );
}
#endif

static void cmd_show_usage(nrf_cli_t const * p_cli, size_t argc, char **argv)
{
    UNUSED_PARAMETER(argc);
    UNUSED_PARAMETER(argv);

    nrf_cli_info(p_cli, "reset    -- reset the device "  );
    nrf_cli_info(p_cli, "gpio     -- test the led "  );
    nrf_cli_info(p_cli, "bleping  -- ble ping test "  );
    nrf_cli_info(p_cli, "blepong  -- ble pong test "  );
    nrf_cli_info(p_cli, "blectx   -- ble ctx test "  );
    nrf_cli_info(p_cli, "battery  -- get the battery volt "  );
    nrf_cli_info(p_cli, "cache    -- enable/disable cache function "  );
}

#ifdef SUBG_CMD 
NRF_CLI_CREATE_STATIC_SUBCMD_SET(m_usage_cmd)
{

    NRF_CLI_CMD(usage,   NULL, "show all cmd", cmd_show_usage),

    NRF_CLI_SUBCMD_SET_END

};
#endif

NRF_CLI_CMD_REGISTER(help, NULL, "usage", cmd_show_usage);




/**用法
 * demo:
 * 
 *  >>battery vol
 *  battery voltage = 3330 mv
 */



