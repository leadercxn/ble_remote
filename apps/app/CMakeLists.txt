set(target "app")

version_target(${target})

set(SDK_SOURCE_FILES
    ${SDK_ROOT}/components/libraries/button/app_button.c
    ${SDK_ROOT}/components/libraries/util/nrf_assert.c
    ${SDK_ROOT}/components/libraries/scheduler/app_scheduler.c
    ${SDK_ROOT}/components/libraries/timer/app_timer2.c
    ${SDK_ROOT}/components/libraries/timer/drv_rtc.c
    ${SDK_ROOT}/components/libraries/crc16/crc16.c
    ${SDK_ROOT}/components/libraries/fds/fds.c
    ${SDK_ROOT}/components/libraries/hardfault/hardfault_implementation.c
    ${SDK_ROOT}/components/libraries/atomic_fifo/nrf_atfifo.c
    ${SDK_ROOT}/components/libraries/atomic_flags/nrf_atflags.c
    ${SDK_ROOT}/components/libraries/atomic/nrf_atomic.c
    ${SDK_ROOT}/components/libraries/balloc/nrf_balloc.c
    ${SDK_ROOT}/components/libraries/fstorage/nrf_fstorage.c
    ${SDK_ROOT}/components/libraries/fstorage/nrf_fstorage_sd.c
    ${SDK_ROOT}/components/libraries/memobj/nrf_memobj.c
    ${SDK_ROOT}/components/libraries/pwr_mgmt/nrf_pwr_mgmt.c
    ${SDK_ROOT}/components/libraries/ringbuf/nrf_ringbuf.c
    ${SDK_ROOT}/components/libraries/queue/nrf_queue.c
    ${SDK_ROOT}/components/libraries/experimental_section_vars/nrf_section_iter.c
    ${SDK_ROOT}/components/libraries/sortlist/nrf_sortlist.c
    ${SDK_ROOT}/components/libraries/strerror/nrf_strerror.c
    ${SDK_ROOT}/components/libraries/scheduler/app_scheduler.c

    ${SDK_ROOT}/components/libraries/log/src/nrf_log_backend_rtt.c
    ${SDK_ROOT}/components/libraries/log/src/nrf_log_backend_serial.c
    ${SDK_ROOT}/components/libraries/log/src/nrf_log_backend_uart.c
    ${SDK_ROOT}/components/libraries/log/src/nrf_log_default_backends.c
    ${SDK_ROOT}/components/libraries/log/src/nrf_log_frontend.c
    ${SDK_ROOT}/components/libraries/log/src/nrf_log_str_formatter.c

    ${SDK_ROOT}/components/ble/common/ble_conn_params.c
    ${SDK_ROOT}/components/ble/common/ble_conn_state.c
    ${SDK_ROOT}/components/ble/common/ble_srv_common.c
    ${SDK_ROOT}/components/ble/nrf_ble_gatt/nrf_ble_gatt.c
    ${SDK_ROOT}/components/ble/nrf_ble_qwr/nrf_ble_qwr.c

    ${SDK_ROOT}/components/softdevice/common/nrf_sdh.c
    ${SDK_ROOT}/components/softdevice/common/nrf_sdh_ble.c
    ${SDK_ROOT}/components/softdevice/common/nrf_sdh_soc.c

    ${SDK_ROOT}/modules/nrfx/soc/nrfx_atomic.c
    ${SDK_ROOT}/modules/nrfx/drivers/src/prs/nrfx_prs.c
    ${SDK_ROOT}/modules/nrfx/drivers/src/nrfx_clock.c
    ${SDK_ROOT}/modules/nrfx/drivers/src/nrfx_gpiote.c
    ${SDK_ROOT}/modules/nrfx/drivers/src/nrfx_uart.c
    ${SDK_ROOT}/modules/nrfx/drivers/src/nrfx_uarte.c
    ${SDK_ROOT}/modules/nrfx/drivers/src/nrfx_rng.c

    ${SDK_ROOT}/integration/nrfx/legacy/nrf_drv_clock.c
    ${SDK_ROOT}/integration/nrfx/legacy/nrf_drv_uart.c
    ${SDK_ROOT}/integration/nrfx/legacy/nrf_drv_rng.c

    ${SDK_ROOT}/external/fprintf/nrf_fprintf.c
    ${SDK_ROOT}/external/fprintf/nrf_fprintf_format.c
    ${SDK_ROOT}/external/utf_converter/utf.c
)

set(SDK_INCLUDE_DIRS
    ${SDK_ROOT}/components

    ${SDK_ROOT}/components/ble/common
    ${SDK_ROOT}/components/ble/nrf_ble_gatt
    ${SDK_ROOT}/components/ble/nrf_ble_qwr
    ${SDK_ROOT}/components/ble/peer_manager
    ${SDK_ROOT}/components/ble/ble_racp
    ${SDK_ROOT}/components/ble/ble_link_ctx_manager
    ${SDK_ROOT}/components/ble/ble_advertising

    ${SDK_ROOT}/components/libraries/atomic_fifo
    ${SDK_ROOT}/components/libraries/atomic_flags
    ${SDK_ROOT}/components/libraries/pwm
    ${SDK_ROOT}/components/libraries/log
    ${SDK_ROOT}/components/libraries/fstorage
    ${SDK_ROOT}/components/libraries/fds
    ${SDK_ROOT}/components/libraries/mutex
    ${SDK_ROOT}/components/libraries/gpiote
    ${SDK_ROOT}/components/libraries/bootloader/ble_dfu
    ${SDK_ROOT}/components/libraries/queue
    ${SDK_ROOT}/components/libraries/pwr_mgmt
    ${SDK_ROOT}/components/libraries/bsp
    ${SDK_ROOT}/components/libraries/mpu
    ${SDK_ROOT}/components/libraries/experimental_section_vars
    ${SDK_ROOT}/components/libraries/delay
    ${SDK_ROOT}/components/libraries/svc
    ${SDK_ROOT}/components/libraries/atomic
    ${SDK_ROOT}/components/libraries/scheduler
    ${SDK_ROOT}/components/libraries/crc16
    ${SDK_ROOT}/components/libraries/util
    ${SDK_ROOT}/components/libraries/balloc
    ${SDK_ROOT}/components/libraries/hardfault
    ${SDK_ROOT}/components/libraries/timer
    ${SDK_ROOT}/components/libraries/sortlist
    ${SDK_ROOT}/components/libraries/strerror
    ${SDK_ROOT}/components/libraries/mem_manager
    ${SDK_ROOT}/components/libraries/memobj
    ${SDK_ROOT}/components/libraries/ringbuf
    ${SDK_ROOT}/components/libraries/button
    ${SDK_ROOT}/components/libraries/stack_guard
    ${SDK_ROOT}/components/libraries/log/src
    ${SDK_ROOT}/components/libraries/serial

    ${SDK_ROOT}/components/toolchain/cmsis/include

    ${SDK_ROOT}/external/fprintf
    ${SDK_ROOT}/external/segger_rtt

    ${SDK_ROOT}/integration/nrfx/legacy
    ${SDK_ROOT}/integration/nrfx

    ${SDK_ROOT}/modules/nrfx/drivers/include
)

file(GLOB APP_SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.c

    ${CMAKE_SOURCE_DIR}/libraries/interface/*.c
)

set(APP_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/config

    ${CMAKE_SOURCE_DIR}/libraries/interface
)

add_executable(${target}
    ${APP_SOURCE_FILES}
    ${SDK_SOURCE_FILES}
    ${${PLATFORM}_SOURCE_FILES}
    ${${nRF5_SDK_VERSION}_SOURCE_FILES}
)

target_include_directories(${target} PUBLIC
    ${APP_INCLUDE_DIRS}
    ${SDK_INCLUDE_DIRS}
)

set_target_link_options(${target}
    ${CMAKE_CURRENT_SOURCE_DIR}/linker/${PLATFORM}_${SOFTDEVICE})

target_compile_definitions(${target} PRIVATE
    ${CONFIG_BUILD_TYPE}
    PROJECT_NAME="${PROJECT_NAME}"
    APP_VERSION_HASH="${${target}_VERSION}_${BUILD_HASH}"
    APP_TIMER_V2
    APP_TIMER_V2_RTC1_ENABLED
    USE_APP_CONFIG
    __STACK_SIZE=3072
    __HEAP_SIZE=2048
)

create_hex(${target})
add_flash_target(${target})
