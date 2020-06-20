#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "nrf_cli.h"
#include "nrf_cli_types.h"

#include "storage_cfg.h"
#include "storage_handler.h"

static nrf_cli_t const *mp_cli = NULL ;                 //接收来自上一层传递的串口指针    
static uint32_t  m_cmd_cache =  0 ;
static uint32_t  old_m_cmd_cache = 0 ; 

extern uint8_t g_is_cache_on ;

static void usage(void)
{
    nrf_cli_info( mp_cli,"Available options: " );
    nrf_cli_info( mp_cli," -h print this help " );
    nrf_cli_info( mp_cli," 1  enable cache function" );
    nrf_cli_info( mp_cli," 0  disable cache function" );
}

static void cmd_cache_test(nrf_cli_t const * p_cli, size_t argc, char **argv)
{
    mp_cli = p_cli ;  

    if( argc != 2 )
    {
        nrf_cli_info(mp_cli,"error cmd format" );
        return ;
    }

    for( uint8_t i = 1; i < argc; )
    {
        if( strcmp( argv[i], "-h" ) == 0 )
        {
            i++;
            usage();
            return ;
        }
        else if( strcmp( argv[i], "1" ) == 0 )
        {
            m_cmd_cache = 1 ;
            i++;
        }
        else if( strcmp( argv[i], "0" ) == 0 )
        {
            m_cmd_cache = 0 ;
            i++;
        }
        else
        {
            nrf_cli_info(mp_cli,"cache: invalid arg" );
            return ;
        }
    }

    if(m_cmd_cache == 1)
    {
        g_is_cache_on = 1 ; 
        nrf_cli_info(mp_cli,"cache enable" );
    }
    else
    {
        g_is_cache_on = 0 ; 
        nrf_cli_info(mp_cli,"cache disable" );
    }

    fstorage_read( CACHE_ADDR , &old_m_cmd_cache , sizeof(uint32_t) );

    if( old_m_cmd_cache != m_cmd_cache )
    {
        fstorage_erase( CACHE_ADDR, 1 );
        fstorage_write( CACHE_ADDR, &m_cmd_cache , sizeof(uint32_t)  );
    }

}

NRF_CLI_CMD_REGISTER(cache, NULL, "cache function", cmd_cache_test);








