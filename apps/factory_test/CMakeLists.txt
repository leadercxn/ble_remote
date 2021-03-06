#set(target "template_${PLATFORM}_${SOFTDEVICE}")
set(target "factory")

file(GLOB USER_SOURCE_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/main.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/cli.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/radio_config.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/storage_handler.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../lib/interface/battery_handler.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../lib/interface/storage_handler.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../lib/platform/pl_adc.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../lib/platform/pl_utils.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../lib/platform/pl_timer.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/test_items/battery_test.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/test_items/ble_ping.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/test_items/ble_pong.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/test_items/gpio_test.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/test_items/ble_ctx.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/test_items/cache.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/test_items/usage.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/user_cli/cli_utils_cmds.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/user_cli/nrf_cli.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/user_cli/uart/nrf_cli_uart.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/proto/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../app/led_handler.c"

)

set(USER_INCLUDE_DIRS
    "${CMAKE_CURRENT_SOURCE_DIR}/config"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../lib/platform/nrf52/timer"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../lib/platform"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../lib/interface"
    "${CMAKE_CURRENT_SOURCE_DIR}/user_board"
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/proto"
    "${CMAKE_CURRENT_SOURCE_DIR}/user_cli"
    "${CMAKE_CURRENT_SOURCE_DIR}/user_cli/uart"
    "${CMAKE_CURRENT_SOURCE_DIR}/../app"

)


set(SDK_LIBRARY_SOURCE_FILES
    "${SDK_ROOT}/components/ble/ble_services/ble_lbs/ble_lbs.c"
 #   "${SDK_ROOT}/components/ble/ble_services/ble_lbs_c/ble_lbs_c.c"
    "${SDK_ROOT}/integration/nrfx/legacy/nrf_drv_clock.c"
 #   "${SDK_ROOT}/integration/nrfx/legacy/nrf_drv_rng.c"
    "${SDK_ROOT}/integration/nrfx/legacy/nrf_drv_uart.c"
    "${SDK_ROOT}/modules/nrfx/soc/nrfx_atomic.c"
    "${SDK_ROOT}/modules/nrfx/drivers/src/nrfx_power.c"
    "${SDK_ROOT}/modules/nrfx/drivers/src/nrfx_clock.c"
    "${SDK_ROOT}/modules/nrfx/drivers/src/nrfx_gpiote.c"
    "${SDK_ROOT}/modules/nrfx/drivers/src/prs/nrfx_prs.c"
 #   "${SDK_ROOT}/modules/nrfx/drivers/src/nrfx_rng.c"
    "${SDK_ROOT}/modules/nrfx/drivers/src/nrfx_uart.c"
    "${SDK_ROOT}/modules/nrfx/drivers/src/nrfx_uarte.c"
    "${SDK_ROOT}/modules/nrfx/drivers/src/nrfx_saadc.c"
    "${SDK_ROOT}/modules/nrfx/hal/nrf_nvmc.c"
    "${SDK_ROOT}/components/boards/boards.c"
 #   "${SDK_ROOT}/components/libraries/bsp/bsp.c"
 #   "${SDK_ROOT}/components/libraries/bsp/bsp_btn_ble.c"
    "${SDK_ROOT}/components/libraries/button/app_button.c"
 #    "${SDK_ROOT}/components/libraries/cli/cli_utils_cmds.c"
 #    "${SDK_ROOT}/components/libraries/cli/nrf_cli.c"
 #    "${SDK_ROOT}/components/libraries/cli/uart/nrf_cli_uart.c"

 #   "${SDK_ROOT}/components/libraries/crypto/nrf_crypto_aead.c"
 #   "${SDK_ROOT}/components/libraries/crypto/nrf_crypto_aes.c"
 #   "${SDK_ROOT}/components/libraries/crypto/nrf_crypto_aes_shared.c"
 #   "${SDK_ROOT}/components/libraries/crypto/nrf_crypto_ecc.c"
 #   "${SDK_ROOT}/components/libraries/crypto/nrf_crypto_ecdh.c"
 #   "${SDK_ROOT}/components/libraries/crypto/nrf_crypto_ecdsa.c"
 #   "${SDK_ROOT}/components/libraries/crypto/nrf_crypto_eddsa.c"
 #   "${SDK_ROOT}/components/libraries/crypto/nrf_crypto_error.c"
 #   "${SDK_ROOT}/components/libraries/crypto/nrf_crypto_hash.c"
 #   "${SDK_ROOT}/components/libraries/crypto/nrf_crypto_hkdf.c"
 #   "${SDK_ROOT}/components/libraries/crypto/nrf_crypto_hmac.c"
 #   "${SDK_ROOT}/components/libraries/crypto/nrf_crypto_init.c"
 #   "${SDK_ROOT}/components/libraries/crypto/nrf_crypto_rng.c"
 #   "${SDK_ROOT}/components/libraries/crypto/nrf_crypto_shared.c"
 #   "${SDK_ROOT}/components/libraries/crypto/backend/nrf_hw/nrf_hw_backend_init.c"
 #   "${SDK_ROOT}/components/libraries/crypto/backend/nrf_hw/nrf_hw_backend_rng.c"
 #   "${SDK_ROOT}/components/libraries/crypto/backend/nrf_hw/nrf_hw_backend_rng_mbedtls.c"
 #   "${SDK_ROOT}/components/libraries/crypto/backend/cc310/cc310_backend_aes.c"
 #   "${SDK_ROOT}/components/libraries/crypto/backend/cc310/cc310_backend_aes_aead.c"
 #   "${SDK_ROOT}/components/libraries/crypto/backend/cc310/cc310_backend_chacha_poly_aead.c"
 #   "${SDK_ROOT}/components/libraries/crypto/backend/cc310/cc310_backend_ecc.c"
 #   "${SDK_ROOT}/components/libraries/crypto/backend/cc310/cc310_backend_ecdh.c"
 #   "${SDK_ROOT}/components/libraries/crypto/backend/cc310/cc310_backend_ecdsa.c"
 #   "${SDK_ROOT}/components/libraries/crypto/backend/cc310/cc310_backend_eddsa.c"
 #   "${SDK_ROOT}/components/libraries/crypto/backend/cc310/cc310_backend_hash.c"
 #   "${SDK_ROOT}/components/libraries/crypto/backend/cc310/cc310_backend_hmac.c"
 #   "${SDK_ROOT}/components/libraries/crypto/backend/cc310/cc310_backend_init.c"
 #   "${SDK_ROOT}/components/libraries/crypto/backend/cc310/cc310_backend_mutex.c"
 #   "${SDK_ROOT}/components/libraries/crypto/backend/cc310/cc310_backend_rng.c"
 #   "${SDK_ROOT}/components/libraries/crypto/backend/cc310/cc310_backend_shared.c"
 #   "${SDK_ROOT}/components/libraries/crypto/backend/oberon/oberon_backend_chacha_poly_aead.c"
 #   "${SDK_ROOT}/components/libraries/crypto/backend/oberon/oberon_backend_ecc.c"
 #   "${SDK_ROOT}/components/libraries/crypto/backend/oberon/oberon_backend_ecdh.c"
 #   "${SDK_ROOT}/components/libraries/crypto/backend/oberon/oberon_backend_ecdsa.c"
 #   "${SDK_ROOT}/components/libraries/crypto/backend/oberon/oberon_backend_eddsa.c"
 #   "${SDK_ROOT}/components/libraries/crypto/backend/oberon/oberon_backend_hash.c"
 #   "${SDK_ROOT}/components/libraries/crypto/backend/oberon/oberon_backend_hmac.c"
    "${SDK_ROOT}/components/libraries/atomic_fifo/nrf_atfifo.c"
    "${SDK_ROOT}/components/libraries/atomic_flags/nrf_atflags.c"
    "${SDK_ROOT}/components/libraries/atomic/nrf_atomic.c"
    "${SDK_ROOT}/components/libraries/balloc/nrf_balloc.c"
    "${SDK_ROOT}/components/libraries/crc16/crc16.c"
    "${SDK_ROOT}/components/libraries/experimental_section_vars/nrf_section_iter.c"
    "${SDK_ROOT}/components/libraries/fds/fds.c"
    "${SDK_ROOT}/components/libraries/fstorage/nrf_fstorage.c"
    "${SDK_ROOT}/components/libraries/fstorage/nrf_fstorage_nvmc.c"
    "${SDK_ROOT}/components/libraries/hardfault/hardfault_implementation.c"
    "${SDK_ROOT}/components/libraries/log/src/nrf_log_backend_rtt.c"
    "${SDK_ROOT}/components/libraries/log/src/nrf_log_backend_serial.c"
    "${SDK_ROOT}/components/libraries/log/src/nrf_log_backend_uart.c"
    "${SDK_ROOT}/components/libraries/log/src/nrf_log_default_backends.c"
    "${SDK_ROOT}/components/libraries/log/src/nrf_log_frontend.c"
    "${SDK_ROOT}/components/libraries/log/src/nrf_log_str_formatter.c"
    "${SDK_ROOT}/components/libraries/mem_manager/mem_manager.c"
    "${SDK_ROOT}/components/libraries/memobj/nrf_memobj.c"
    "${SDK_ROOT}/components/libraries/pwr_mgmt/nrf_pwr_mgmt.c"
   "${SDK_ROOT}/components/libraries/queue/nrf_queue.c"
    "${SDK_ROOT}/components/libraries/ringbuf/nrf_ringbuf.c"
    "${SDK_ROOT}/components/libraries/scheduler/app_scheduler.c"
    "${SDK_ROOT}/components/libraries/sortlist/nrf_sortlist.c"
    "${SDK_ROOT}/components/libraries/strerror/nrf_strerror.c"
 #   "${SDK_ROOT}/components/libraries/sensorsim/sensorsim.c"
    "${SDK_ROOT}/components/libraries/timer/app_timer2.c"
    "${SDK_ROOT}/components/libraries/timer/drv_rtc.c"
    "${SDK_ROOT}/components/libraries/util/app_error_weak.c"
    "${SDK_ROOT}/components/libraries/util/nrf_assert.c"
    "${SDK_ROOT}/components/libraries/util/app_util_platform.c"
 #   "${SDK_ROOT}/components/libraries/util/app_error.c"
    "${SDK_ROOT}/components/softdevice/common/nrf_sdh.c"
    "${SDK_ROOT}/components/softdevice/common/nrf_sdh_ble.c"
    "${SDK_ROOT}/components/softdevice/common/nrf_sdh_soc.c"
 #   "${SDK_ROOT}/components/ble/peer_manager/auth_status_tracker.c"
    "${SDK_ROOT}/components/ble/common/ble_advdata.c"
    "${SDK_ROOT}/components/ble/ble_advertising/ble_advertising.c"
    "${SDK_ROOT}/components/ble/ble_db_discovery/ble_db_discovery.c"
    "${SDK_ROOT}/components/ble/common/ble_conn_params.c"
    "${SDK_ROOT}/components/ble/common/ble_conn_state.c"
    "${SDK_ROOT}/components/ble/common/ble_srv_common.c"
 #   "${SDK_ROOT}/components/ble/peer_manager/gatt_cache_manager.c"
 #   "${SDK_ROOT}/components/ble/peer_manager/gatts_cache_manager.c"
 #   "${SDK_ROOT}/components/ble/peer_manager/id_manager.c"
    "${SDK_ROOT}/components/ble/nrf_ble_gatt/nrf_ble_gatt.c"
 #   "${SDK_ROOT}/components/ble/peer_manager/nrf_ble_lesc.c"
    "${SDK_ROOT}/components/ble/nrf_ble_qwr/nrf_ble_qwr.c"
    "${SDK_ROOT}/components/ble/nrf_ble_scan/nrf_ble_scan.c"
    "${SDK_ROOT}/components/ble/nrf_ble_gq/nrf_ble_gq.c"
 #   "${SDK_ROOT}/components/ble/peer_manager/peer_data_storage.c"
 #   "${SDK_ROOT}/components/ble/peer_manager/peer_database.c"
 #   "${SDK_ROOT}/components/ble/peer_manager/peer_id.c"
 #   "${SDK_ROOT}/components/ble/peer_manager/peer_manager.c"
 #   "${SDK_ROOT}/components/ble/peer_manager/peer_manager_handler.c"
 #   "${SDK_ROOT}/components/ble/peer_manager/pm_buffer.c"
 #   "${SDK_ROOT}/components/ble/peer_manager/security_dispatcher.c"
 #   "${SDK_ROOT}/components/ble/peer_manager/security_manager.c"
 #   "${SDK_ROOT}/components/ble/ble_services/ble_bas/ble_bas.c"
 #   "${SDK_ROOT}/components/ble/ble_services/ble_dis/ble_dis.c"
 #   "${SDK_ROOT}/components/ble/ble_services/ble_hrs/ble_hrs.c"
    CACHE INTERNAL ""
)

set(SDK_LIBRARY_INCLUDE_DIRS
    "${SDK_ROOT}/components"
    "${SDK_ROOT}/components/ble/ble_services/ble_lbs"
    "${SDK_ROOT}/components/ble/ble_services/ble_lbs_c"
    "${SDK_ROOT}/components/ble/ble_advertising"
    "${SDK_ROOT}/components/ble/ble_db_discovery"
    "${SDK_ROOT}/components/ble/ble_dtm"
    "${SDK_ROOT}/components/ble/common"
    "${SDK_ROOT}/components/ble/nrf_ble_gatt"
    "${SDK_ROOT}/components/ble/nrf_ble_qwr"
    "${SDK_ROOT}/components/ble/nrf_ble_scan"
    "${SDK_ROOT}/components/ble/nrf_ble_gq"
    "${SDK_ROOT}/components/ble/peer_manager"
    "${SDK_ROOT}/components/ble/ble_racp"
    "${SDK_ROOT}/components/ble/ble_services/ble_bas"
    "${SDK_ROOT}/components/ble/ble_services/ble_dis"
    "${SDK_ROOT}/components/ble/ble_services/ble_hrs"
    "${SDK_ROOT}/components/boards"
    "${SDK_ROOT}/components/libraries/crypto"
    "${SDK_ROOT}/components/libraries/crypto/backend/cc310"
    "${SDK_ROOT}/components/libraries/crypto/backend/cc310_bl"
    "${SDK_ROOT}/components/libraries/crypto/backend/cifra"
    "${SDK_ROOT}/components/libraries/crypto/backend/micro_ecc"
    "${SDK_ROOT}/components/libraries/crypto/backend/mbedtls"
    "${SDK_ROOT}/components/libraries/crypto/backend/nrf_hw"
    "${SDK_ROOT}/components/libraries/crypto/backend/nrf_sw"
    "${SDK_ROOT}/components/libraries/crypto/backend/oberon"
    "${SDK_ROOT}/components/libraries/crypto/backend/optiga"
    "${SDK_ROOT}/components/libraries/atomic"
    "${SDK_ROOT}/components/libraries/atomic_fifo"
    "${SDK_ROOT}/components/libraries/atomic_flags"
    "${SDK_ROOT}/components/libraries/balloc"
    "${SDK_ROOT}/components/libraries/bootloader/ble_dfu"
    "${SDK_ROOT}/components/libraries/bsp"
    "${SDK_ROOT}/components/libraries/button"
    "${SDK_ROOT}/components/libraries/csense_drv"
    "${SDK_ROOT}/components/libraries/csense"
 #    "${SDK_ROOT}/components/libraries/cli"
 #    "${SDK_ROOT}/components/libraries/cli/uart"
    "${SDK_ROOT}/components/libraries/crc16"
    "${SDK_ROOT}/components/libraries/crc32"
    "${SDK_ROOT}/components/libraries/ecc"
    "${SDK_ROOT}/components/libraries/experimental_task_manager"
    "${SDK_ROOT}/components/libraries/fds"
    "${SDK_ROOT}/components/libraries/fstorage"
    "${SDK_ROOT}/components/libraries/gfx"
    "${SDK_ROOT}/components/libraries/gpiote"
    "${SDK_ROOT}/components/libraries/hardfault"
    "${SDK_ROOT}/components/libraries/hci"
    "${SDK_ROOT}/components/libraries/led_softblink"
    "${SDK_ROOT}/components/libraries/low_power_pwm"
    "${SDK_ROOT}/components/libraries/memobj"
    "${SDK_ROOT}/components/libraries/mem_manager"
    "${SDK_ROOT}/components/libraries/mpu"
    "${SDK_ROOT}/components/libraries/mutex"
    "${SDK_ROOT}/components/libraries/pwm"
    "${SDK_ROOT}/components/libraries/pwr_mgmt"
    "${SDK_ROOT}/components/libraries/queue"
    "${SDK_ROOT}/components/libraries/ringbuf"
    "${SDK_ROOT}/components/libraries/sdcard"
    "${SDK_ROOT}/components/libraries/scheduler"
    "${SDK_ROOT}/components/libraries/sensorsim"
    "${SDK_ROOT}/components/libraries/slip"
    "${SDK_ROOT}/components/libraries/svc"
    "${SDK_ROOT}/components/libraries/stack_info"
    "${SDK_ROOT}/components/libraries/stack_guard"
    "${SDK_ROOT}/components/libraries/strerror"
    "${SDK_ROOT}/components/libraries/sortlist"
    "${SDK_ROOT}/components/libraries/spi_mngr"
    "${SDK_ROOT}/components/libraries/twi_mngr"
    "${SDK_ROOT}/components/libraries/twi_sensor"
    "${SDK_ROOT}/components/libraries/usbd"
    "${SDK_ROOT}/components/libraries/usbd/class/audio"
    "${SDK_ROOT}/components/libraries/usbd/class/cdc"
    "${SDK_ROOT}/components/libraries/usbd/class/cdc/acm"
    "${SDK_ROOT}/components/libraries/usbd/class/hid/generic"
    "${SDK_ROOT}/components/libraries/usbd/class/msc"
    "${SDK_ROOT}/components/libraries/usbd/class/hid"
    "${SDK_ROOT}/components/libraries/usbd/class/hid/kbd"
    "${SDK_ROOT}/components/libraries/usbd/class/hid/mouse"
    "${SDK_ROOT}/components/libraries/util"
    "${SDK_ROOT}/components/softdevice/common"
   # "${SDK_ROOT}/components/softdevice/s132/headers"
    "${SDK_ROOT}/components/softdevice/s112/headers"
    "${SDK_ROOT}/components/softdevice/s112/headers/nrf52"

    "${SDK_ROOT}/modules/nrfx"
    "${SDK_ROOT}/modules/nrfx/hal"
    "${SDK_ROOT}/modules/nrfx/drivers"
    "${SDK_ROOT}/modules/nrfx/drivers/include"
    "${SDK_ROOT}/modules/nrfx/mdk"
    "${SDK_ROOT}/modules/nrfx/hal"

    "${SDK_ROOT}/integration/nrfx"
    "${SDK_ROOT}/integration/nrfx/legacy"
    CACHE INTERNAL ""
)

set(SDK_EXTERNAL_SOURCE_FILES
    "${SDK_ROOT}/external/fprintf/nrf_fprintf.c"
    "${SDK_ROOT}/external/fprintf/nrf_fprintf_format.c"
    "${SDK_ROOT}/external/mbedtls/library/aes.c"
    "${SDK_ROOT}/external/mbedtls/library/ctr_drbg.c"
#    "${SDK_ROOT}/external/nano-pb/pb_common.c"
#    "${SDK_ROOT}/external/nano-pb/pb_decode.c"
#    "${SDK_ROOT}/external/nano-pb/pb_encode.c"
    "${SDK_ROOT}/external/utf_converter/utf.c"
    "${SDK_ROOT}/external/segger_rtt/SEGGER_RTT.c"
#    "${SDK_ROOT}/external/segger_rtt/SEGGER_RTT_Syscalls_GCC.c"    #这里最好加判断分支用哪个原文件
#    "${SDK_ROOT}/external/segger_rtt/SEGGER_RTT_Syscalls_KEIL.c"
    "${SDK_ROOT}/external/segger_rtt/SEGGER_RTT_printf.c"
    "${SDK_ROOT}/external/fnmatch/fnmatch.c"
    CACHE INTERNAL ""
)

set(SDK_EXTERNAL_INCLUDE_DIRS
    "${SDK_ROOT}/external/fprintf"
    "${SDK_ROOT}/external/mbedtls/include"
    "${SDK_ROOT}/external/nano-pb"
    "${SDK_ROOT}/external/nrf_cc310/include"
    "${SDK_ROOT}/external/nrf_oberon"
    "${SDK_ROOT}/external/nrf_oberon/include"
    "${SDK_ROOT}/external/nrf_tls/mbedtls/nrf_crypto/config"
    "${SDK_ROOT}/external/segger_rtt"
    "${SDK_ROOT}/external/utf_converter"
    "${SDK_ROOT}/external/fnmatch"
    CACHE INTERNAL ""
)




#if (TOOLCHAIN MATCHES "armgcc")
#    set(SDK_EXTERNAL_LIB_FILES
#        "${SDK_ROOT}/external/nrf_cc310/lib/cortex-m4/hard-float/libnrf_cc310_0.9.12.a"
#        "${SDK_ROOT}/external/nrf_oberon/lib/cortex-m4/hard-float/liboberon_3.0.1.a"
#        CACHE INTERNAL ""
#    )
#    set(SDK_EXTERNAL_SOURCE_FILES                                   #根据不同的编译器来选择打印文件
#        "${SDK_ROOT}/external/segger_rtt/SEGGER_RTT_Syscalls_GCC.c"
#    )
#    message(STATUS "using SEGGER_RTT_Syscalls_GCC ")
#elseif (TOOLCHAIN STREQUAL "armcc")
#    set(SDK_EXTERNAL_LIB_FILES
#    "${SDK_ROOT}/external/nrf_cc310/lib/cortex-m4/hard-float/short-wchar/nrf_cc310_0.9.12.lib"
#    "${SDK_ROOT}/external/nrf_oberon/lib/cortex-m4/hard-float/short-wchar/oberon_3.0.1.lib"
#    CACHE INTERNAL ""
#    )
#    set(SDK_EXTERNAL_SOURCE_FILES                                   #根据不同的编译器来选择打印文件
#        "${SDK_ROOT}/external/segger_rtt/SEGGER_RTT_Syscalls_KEIL.c"
#    )
#    message(STATUS "using SEGGER_RTT_Syscalls_KEIL ")
#endif()












add_executable(${target}
    ${USER_SOURCE_FILES}
    ${SDK_LIBRARY_SOURCE_FILES}
    ${SDK_EXTERNAL_SOURCE_FILES}
    ${${PLATFORM}_SOURCE_FILES}
    ${${nRF5_SDK_VERSION}_SOURCE_FILES})

target_include_directories(${target} PUBLIC
    ${USER_INCLUDE_DIRS}
    ${SDK_LIBRARY_INCLUDE_DIRS}
    ${SDK_EXTERNAL_INCLUDE_DIRS}
    ${${SOFTDEVICE}_INCLUDE_DIRS}
    ${${PLATFORM}_INCLUDE_DIRS}
    ${${nRF5_SDK_VERSION}_INCLUDE_DIRS})

set_target_link_options(${target}
    ${CMAKE_CURRENT_SOURCE_DIR}/linker/${PLATFORM}_${SOFTDEVICE})

target_compile_options(${target} PUBLIC
    ${${ARCH}_DEFINES})

if(TOOLCHAIN MATCHES "armgcc")
    set(COMPILER_DEFS 
    -DMBEDTLS_CONFIG_FILE=\"nrf_crypto_mbedtls_config.h\"
    -DNRF_CRYPTO_MAX_INSTANCE_COUNT=1)
elseif(TOOLCHAIN STREQUAL "armcc")
    set(COMPILER_DEFS 
    -DFLOAT_ABI_HARD)
endif()

target_compile_definitions(${target} PUBLIC
    ${USER_DEFINITIONS}
    -DAPP_TIMER_V2
    -DAPP_TIMER_V2_RTC1_ENABLED
    -DBLE_STACK_SUPPORT_REQD
#    -DBOARD_PCA10056
    -DBOARD_PCA10040
#    -DBOARD_CUSTOM
    -DBLE_LBS
    -DCONFIG_GPIO_AS_PINRESET
#    -DNRFX_COREDEP_DELAY_US_LOOP_CYCLES=3
#    -DDEVELOP_IN_NRF52832
    ${COMPILER_DEFS}                    # -DFLOAT_ABI_HARD
    ${${PLATFORM}_DEFINES}              # -DNRF52_SERIES;-DNRF52832;-DNRF52832_XXAA;-DNRF52_PAN_74
    ${${SOFTDEVICE}_DEFINES})           # -DS132;-DSOFTDEVICE_PRESENT;-DNRF_SD_BLE_API_VERSION=7

message(STATUS "COMPILER_DEFS:${COMPILER_DEFS}")
message(STATUS "PLATFORM_DEFINES:${${PLATFORM}_DEFINES}")
message(STATUS "SOFTDEVICE_DEFINES:${${SOFTDEVICE}_DEFINES}")
message(STATUS "TOOLCHAIN:${TOOLCHAIN}")

target_link_libraries(${target}
    ${SDK_EXTERNAL_LIB_FILES}
)

create_hex(${target})
add_flash_target(${target})


