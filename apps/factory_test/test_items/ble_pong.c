#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "nrf_error.h"
#include "nrf_cli.h"
#include "nrf_cli_types.h"
#include "nrf_cli_uart.h"

#include "radio_config.h"
#include "pl_timer.h"


static ble_radio_event_handler_t m_radio_event_handler;
static nrf_cli_t const *mp_cli = NULL ;                 //接收来自上一层传递的串口指针     


#define BLE_PONG_STATE_IDLE     0
#define BLE_PONG_STATE_RX       1
#define BLE_PONG_STATE_TX       2

static ble_radio_config_t m_pong_tx_config;
static ble_radio_config_t m_pong_rx_config;

static uint32_t m_rx_packet = 0;
static uint32_t m_tx_packet = 0;

static uint32_t m_ble_pong_state = BLE_PONG_STATE_IDLE;




static void cmd_ble_pong(nrf_cli_t const * p_cli, size_t argc, char **argv)
{
    UNUSED_PARAMETER(argc);
    UNUSED_PARAMETER(argv);
}

static void usage(void)
{
    nrf_cli_info(mp_cli,"Available options:");
    nrf_cli_info(mp_cli," -h print this help");
    nrf_cli_info(mp_cli," -tp <int>   BLE TX_Power [-30,-20,-16,-12,-8,-4,0,4](dBm),default 4");
    nrf_cli_info(mp_cli," -rp <int>   BLE RX_Power [-30,-20,-16,-12,-8,-4,0,4](dBm),default 4");
    nrf_cli_info(mp_cli," -tc <uint>  tx_channel, default 2");
    nrf_cli_info(mp_cli," -rc <uint>  rx_channel, default 2");
}


static void on_ble_radio_event_end_handler(void)
{
    int8_t rssi = 0;
    switch(m_ble_pong_state)
    {
        case BLE_PONG_STATE_RX:
            nrf_cli_info(mp_cli,"OnRxDone");
            if(ble_radio_crc_status_get())
            {
                rssi = ble_radio_rssi_get();
                nrf_cli_info(mp_cli,"data = %d, rssi = %d", m_rx_packet, rssi);

                m_tx_packet = rssi;

                ble_radio_config_set(m_pong_tx_config);
                ble_radio_packet_set((uint8_t *)&m_tx_packet);
                ble_radio_tx_enable();
                m_ble_pong_state = BLE_PONG_STATE_TX;
            }
            else
            {
                nrf_cli_info(mp_cli,"Invalid crc");

                ble_radio_config_set(m_pong_rx_config);
                ble_radio_packet_set((uint8_t *)&m_rx_packet);
                ble_radio_rx_enable();
                m_ble_pong_state = BLE_PONG_STATE_RX;
            }
            break;

        case BLE_PONG_STATE_TX:
            nrf_cli_info(mp_cli,"OnTxDone");

            ble_radio_config_set(m_pong_rx_config);
            ble_radio_packet_set((uint8_t *)&m_rx_packet);
            ble_radio_rx_enable();
            m_ble_pong_state = BLE_PONG_STATE_RX;
            break;

        default:
            break;
    }
}


/**
 * @brief 子命令pong 对应的执行函数 , argc是从子命令开始算的
 */
static void cmd_ble_pong_test(nrf_cli_t const * p_cli, size_t argc, char **argv)
{
    
    uint8_t count = 0;
    int8_t   tx_power = 4;
    uint8_t   tx_channel = 2;
    uint8_t  tx_mode = RADIO_MODE_MODE_Ble_1Mbit;

    int8_t    rx_power = 4;
    uint8_t   rx_channel = 2;
    uint8_t   rx_mode = RADIO_MODE_MODE_Ble_1Mbit;

    mp_cli = p_cli ;    

    for(uint8_t i = 1; i < argc;)
    {
        if(strcmp(argv[i], "-h") == 0)
        {
            i++;
            usage();
            return ;
        }
        else if(strcmp(argv[i], "-tp") == 0)
        {
            i++;
            count = sscanf(argv[i], "%d", (int *)&tx_power);
            if(count != 1)
            {
                nrf_cli_info(mp_cli,"blepong: invalid tx_power arg");
                return ;
            }
            if(tx_power != -30 && tx_power != -20 && tx_power != -16 && tx_power != -12 &&
               tx_power != -8 && tx_power != -4 && tx_power != 0 && tx_power != 4)
            {
                nrf_cli_info(mp_cli,"blepong: invalid  tx_power arg ");
                return ;
            }
            i++;
        }
        else if(strcmp(argv[i], "-rp") == 0)
        {
            i++;
            count = sscanf(argv[i], "%d", (int *)&rx_power);
            if(count != 1)
            {
                nrf_cli_info(mp_cli,"blepong: invalid rx_power arg ");
                return ;
            }
            if(rx_power != -30 && rx_power != -20 && rx_power != -16 && rx_power != -12 &&
               rx_power != -8 && rx_power != -4 && rx_power != 0 && rx_power != 4)
            {
                nrf_cli_info(mp_cli,"blepong: invalid  rx_power arg ");
                return ;
            }
            i++;
        }
        else if(strcmp(argv[i], "-tc") == 0)
        {
            i++;
            tx_channel = atoi(argv[i]);
            i++;

        }
        else if(strcmp(argv[i], "-rc") == 0)
        {
            i++;
            rx_channel = atoi(argv[i]);
            i++;

        }
        else
        {
            nrf_cli_info(mp_cli,"blepong: invalid arg");
            return ;
        }
    }

    nrf_cli_info(mp_cli,"tx_power   = %d ", tx_power);
    nrf_cli_info(mp_cli,"tx_channel = %d ", tx_channel);
    nrf_cli_info(mp_cli,"rx_power   = %d ", rx_power);
    nrf_cli_info(mp_cli,"rx_channel = %d ", rx_channel);

    m_pong_tx_config.power   = tx_power;
    m_pong_tx_config.channel = tx_channel;
    m_pong_tx_config.mode    = tx_mode;

    m_pong_rx_config.power   = rx_power;
    m_pong_rx_config.channel = rx_channel;
    m_pong_rx_config.mode    = rx_mode;

    m_radio_event_handler.on_event_end_handler = on_ble_radio_event_end_handler;
    ble_radio_init(&m_radio_event_handler);
    ble_radio_config_set(m_pong_rx_config);
    ble_radio_packet_set((uint8_t *)&m_rx_packet);
    ble_radio_rx_enable();
    m_ble_pong_state = BLE_PONG_STATE_RX;

    nrf_cli_info(p_cli, "start ble tx pong test\n" );
    while(true)
    {
        uint8_t     data ;
        size_t      cnt ;
        ret_code_t  ret ;

        ret = mp_cli->p_iface->p_api->read( mp_cli->p_iface , &data , sizeof(uint8_t) , &cnt );
        UNUSED_VARIABLE(ret);

        if( data == 0x03 )
        {
            ble_radio_disable();
            
            NVIC_ClearPendingIRQ(RADIO_IRQn);
            NVIC_DisableIRQ(RADIO_IRQn);

            nrf_cli_info(p_cli, "stop ble pong test\n" );
            break ;
        }
    }
}







NRF_CLI_CREATE_STATIC_SUBCMD_SET(m_ble_pong_cmd)
{

    NRF_CLI_CMD( pong,   NULL, "ble pong test", cmd_ble_pong_test ),

    NRF_CLI_SUBCMD_SET_END

};

NRF_CLI_CMD_REGISTER(ble, &m_ble_pong_cmd, "ble pong", cmd_ble_pong);



















