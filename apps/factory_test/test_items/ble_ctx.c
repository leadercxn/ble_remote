#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "nrf_error.h"
#include "nrf_cli.h"
#include "nrf_cli_types.h"
#include "nrf_cli_uart.h"
#include "nrf_gpio.h"
#include "radio_config.h"

#include "board_remote001.h"

static nrf_cli_t const *mp_cli = NULL ;                 //接收来自上一层传递的串口指针    

static void usage(void)
{
    nrf_cli_info( mp_cli,"Available options: " );
    nrf_cli_info( mp_cli," -h print this help " );
    nrf_cli_info( mp_cli," -p <int>   RF power [-30,-20,-16,-12,-8,-4,0,4](dBm),default 4" );
    nrf_cli_info( mp_cli," -c <uint>  Channel, [0 - 100], default 0. Output 'frequency = 2400 + channel' MHz" );
}

/**@brief ble_ctx_test handler
 * 
 */
void cmd_ble_ctx_test( nrf_cli_t const * p_cli , size_t argc, char **argv)
{
    uint8_t count = 0;

    int8_t txpower = 0;
    int8_t channel = 0;
    uint8_t mode = RADIO_MODE_MODE_Ble_1Mbit;

    ble_radio_config_t ctx_config;

    mp_cli = p_cli ;  

    for( uint8_t i = 1; i < argc; )
    {
        if( strcmp( argv[i], "-h" ) == 0 )
        {
            i++;
            usage();
            return ;
        }
        else if( strcmp( argv[i], "-p" ) == 0 )
        {
            i++;
            count = sscanf( argv[i], "%d", ( int * )&txpower );
            if( count != 1 )
            {
                nrf_cli_info(mp_cli,"blectx: invalid arg");
                return ;
            }
            if( txpower != -30 && txpower != -20 && txpower != -16 && txpower != -12 &&
                txpower != -8 && txpower != -4 && txpower != 0 && txpower != 4 )
            {
                nrf_cli_info(mp_cli,"bleping: invalid tx_power arg");
                return ;
            }
            i++;
        }
        else if( strcmp( argv[i], "-c" ) == 0 )
        {
            i++;
            channel = atoi( argv[i] );
            if(channel > 100)
            {
                nrf_cli_info(mp_cli,"blectx: invalid channel" );
                return ;
            }
            i++;
        }
        else
        {
            nrf_cli_info(mp_cli,"blectx: invalid arg" );
            return ;
        }
    }

    nrf_cli_info(mp_cli, "txpower   = %d", txpower );
    nrf_cli_info(mp_cli, "channel   = %d", channel );
    nrf_cli_info(mp_cli, "frequency = %d MH", 2400 + channel );

    ctx_config.power   = txpower;
    ctx_config.channel = channel;
    ctx_config.mode    = mode;

//    ble_radio_switch_init();
    ble_radio_const_carrier_configure();
    ble_radio_config_set(ctx_config);
    ble_radio_tx_enable();

    nrf_cli_info(mp_cli, "blectx: start, ctrl+c to stop " );
    while( 1 )
    {
        uint8_t     data ;
        size_t      cnt ;
        ret_code_t  ret ;

        ret = mp_cli->p_iface->p_api->read( mp_cli->p_iface , &data , sizeof(uint8_t) , &cnt );
        UNUSED_VARIABLE(ret);

        if( 0 == nrf_gpio_pin_read(GUARD_SET_BUTTON) )
        {
            nrf_cli_info(mp_cli, "key down to stop " );
            data = 0x03 ;
        }

        if( data == 0x03 ) // ^C
        {
            NVIC_ClearPendingIRQ(RADIO_IRQn);
            NVIC_DisableIRQ(RADIO_IRQn);

            ble_radio_disable();

            nrf_cli_info(p_cli, "blectx test: end" );
            break;
        }
    }

}


NRF_CLI_CMD_REGISTER(blectx, NULL, "ble ctx test", cmd_ble_ctx_test);






