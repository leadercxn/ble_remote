#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "board_remote001.h"

#include "nrf_cli.h"
#include "nrf_cli_types.h"
#include "nrf_cli_uart.h"
#include "cli.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

/* CLI的使用无非3个关键函数.cli_init => cli_start => cli_process(死循环) */

/**
 * @brief Logger processing queue size.
 * */
#define CLI_LOG_QUEUE_SIZE  (4)

NRF_CLI_UART_DEF(my_cli_uart_transport, 0, 256, 256);       // 实例名,串口号,发送缓冲区大小,接收缓冲区大小. 
NRF_CLI_DEF(my_cli_uart,                                    // 实例名,提示符,发送器指针(当前用串口),换行符,记录日志深度
            ">>",
            &my_cli_uart_transport.transport,
            '\r',
            CLI_LOG_QUEUE_SIZE);


void cli_start(void)
{
    ret_code_t err_code;

    err_code = nrf_cli_start(&my_cli_uart);                                             // 注意，该函数底层调用了app_timer，所以要初始化定时器
    NRF_LOG_INFO("cli_start err_code=%d",err_code);
    APP_ERROR_CHECK(err_code);
}


void cli_init(void)
{
    ret_code_t err_code;

    nrf_drv_uart_config_t uart_config = NRF_DRV_UART_DEFAULT_CONFIG;
    uart_config.pseltxd = UART_TX_PIN;
    uart_config.pselrxd = UART_RX_PIN;
    uart_config.hwfc    = NRF_UART_HWFC_DISABLED;

    my_cli_uart.p_fprintf_ctx->auto_flush = true;

    err_code = nrf_cli_init(&my_cli_uart, &uart_config, true, true, NRF_LOG_SEVERITY_INFO);   // 实例,UART配置,是否使用颜色提示,是否记录日志,日志记录路径. 
                                                                                              // #define NRF_LOG_BACKEND_UART_ENABLED 0  手贱把这里设置为1，BUG怀疑人生
    APP_ERROR_CHECK(err_code);
}

void cli_process(void)
{
    nrf_cli_process(&my_cli_uart);
}


void cli_greeting(void)
{
    nrf_cli_info(&my_cli_uart, "\n");
    nrf_cli_info(&my_cli_uart, "Sensoro BLE remote test");
    nrf_cli_info(&my_cli_uart, "compiled %s", __DATE__);
    nrf_cli_info(&my_cli_uart, "Please input: \"help \" to list all cmd" );
    nrf_cli_info(&my_cli_uart, "Please press the Tab key to see all available commands.");
}



