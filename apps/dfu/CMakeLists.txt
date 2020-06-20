set(target "dfu")

file(GLOB USER_SOURCE_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/main.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/dfu_public_key.c"
)

set(USER_INCLUDE_DIRS
    "${CMAKE_CURRENT_SOURCE_DIR}"
)


set(SDK_LIBRARY_SOURCE_FILES
    "${SDK_ROOT}/components/libraries/util/app_error_weak.c"
    "${SDK_ROOT}/components/libraries/scheduler/app_scheduler.c"
    "${SDK_ROOT}/components/libraries/util/app_util_platform.c"
    "${SDK_ROOT}/components/libraries/crc32/crc32.c"
    "${SDK_ROOT}/components/libraries/mem_manager/mem_manager.c"
    "${SDK_ROOT}/components/libraries/util/nrf_assert.c"
    "${SDK_ROOT}/components/libraries/atomic_fifo/nrf_atfifo.c"
    "${SDK_ROOT}/components/libraries/atomic/nrf_atomic.c"
    "${SDK_ROOT}/components/libraries/balloc/nrf_balloc.c"
    "${SDK_ROOT}/components/libraries/fstorage/nrf_fstorage.c"
    "${SDK_ROOT}/components/libraries/fstorage/nrf_fstorage_nvmc.c"
    "${SDK_ROOT}/components/libraries/fstorage/nrf_fstorage_sd.c"
    "${SDK_ROOT}/components/libraries/queue/nrf_queue.c"
    "${SDK_ROOT}/components/libraries/ringbuf/nrf_ringbuf.c"
    "${SDK_ROOT}/components/libraries/experimental_section_vars/nrf_section_iter.c"
    "${SDK_ROOT}/components/libraries/strerror/nrf_strerror.c"
    "${SDK_ROOT}/components/libraries/sha256/sha256.c"
    "${SDK_ROOT}/components/libraries/crypto/backend/micro_ecc/micro_ecc_backend_ecc.c"
    "${SDK_ROOT}/components/libraries/crypto/backend/micro_ecc/micro_ecc_backend_ecdh.c"
    "${SDK_ROOT}/components/libraries/crypto/backend/micro_ecc/micro_ecc_backend_ecdsa.c"
    "${SDK_ROOT}/components/boards/boards.c"
    "${SDK_ROOT}/modules/nrfx/hal/nrf_nvmc.c"
    "${SDK_ROOT}/modules/nrfx/soc/nrfx_atomic.c"
    "${SDK_ROOT}/components/ble/common/ble_srv_common.c"
    "${SDK_ROOT}/components/libraries/bootloader/nrf_bootloader.c"
    "${SDK_ROOT}/components/libraries/bootloader/nrf_bootloader_app_start.c"
    "${SDK_ROOT}/components/libraries/bootloader/nrf_bootloader_app_start_final.c"
    "${SDK_ROOT}/components/libraries/bootloader/nrf_bootloader_dfu_timers.c"
    "${SDK_ROOT}/components/libraries/bootloader/nrf_bootloader_fw_activation.c"
    "${SDK_ROOT}/components/libraries/bootloader/nrf_bootloader_info.c"
    "${SDK_ROOT}/components/libraries/bootloader/nrf_bootloader_wdt.c"
    "${SDK_ROOT}/components/libraries/crypto/backend/nrf_sw/nrf_sw_backend_hash.c"
    "${SDK_ROOT}/components/libraries/bootloader/dfu/dfu-cc.pb.c"
    "${SDK_ROOT}/components/libraries/bootloader/dfu/nrf_dfu.c"
    "${SDK_ROOT}/components/libraries/bootloader/ble_dfu/nrf_dfu_ble.c"
    "${SDK_ROOT}/components/libraries/bootloader/dfu/nrf_dfu_flash.c"
    "${SDK_ROOT}/components/libraries/bootloader/dfu/nrf_dfu_handling_error.c"
    "${SDK_ROOT}/components/libraries/bootloader/dfu/nrf_dfu_mbr.c"
    "${SDK_ROOT}/components/libraries/bootloader/dfu/nrf_dfu_req_handler.c"
    "${SDK_ROOT}/components/libraries/bootloader/dfu/nrf_dfu_settings.c"
    "${SDK_ROOT}/components/libraries/bootloader/dfu/nrf_dfu_settings_svci.c"
    "${SDK_ROOT}/components/libraries/bootloader/dfu/nrf_dfu_transport.c"
    "${SDK_ROOT}/components/libraries/bootloader/dfu/nrf_dfu_utils.c"
    "${SDK_ROOT}/components/libraries/bootloader/dfu/nrf_dfu_validation.c"
    "${SDK_ROOT}/components/libraries/bootloader/dfu/nrf_dfu_ver_validation.c"
    "${SDK_ROOT}/components/libraries/bootloader/dfu/nrf_dfu_svci.c"
    "${SDK_ROOT}/components/libraries/bootloader/dfu/nrf_dfu_svci_handler.c"
    "${SDK_ROOT}/components/libraries/svc/nrf_svc_handler.c"
    "${SDK_ROOT}/components/softdevice/common/nrf_sdh.c"
    "${SDK_ROOT}/components/softdevice/common/nrf_sdh_ble.c"
    "${SDK_ROOT}/components/softdevice/common/nrf_sdh_soc.c"
    "${SDK_ROOT}/components/libraries/crypto/backend/oberon/oberon_backend_chacha_poly_aead.c"
    "${SDK_ROOT}/components/libraries/crypto/backend/oberon/oberon_backend_ecc.c"
    "${SDK_ROOT}/components/libraries/crypto/backend/oberon/oberon_backend_ecdh.c"
    "${SDK_ROOT}/components/libraries/crypto/backend/oberon/oberon_backend_ecdsa.c"
    "${SDK_ROOT}/components/libraries/crypto/backend/oberon/oberon_backend_eddsa.c"
    "${SDK_ROOT}/components/libraries/crypto/backend/oberon/oberon_backend_hash.c"
    "${SDK_ROOT}/components/libraries/crypto/backend/oberon/oberon_backend_hmac.c"
    "${SDK_ROOT}/components/libraries/crypto/nrf_crypto_ecc.c"
    "${SDK_ROOT}/components/libraries/crypto/nrf_crypto_ecdsa.c"
    "${SDK_ROOT}/components/libraries/crypto/nrf_crypto_hash.c"
    "${SDK_ROOT}/components/libraries/crypto/nrf_crypto_init.c"
    "${SDK_ROOT}/components/libraries/crypto/nrf_crypto_shared.c"
    CACHE INTERNAL ""
)

set(SDK_LIBRARY_INCLUDE_DIRS
    "${SDK_ROOT}/components/libraries/crypto/backend/micro_ecc"
    "${SDK_ROOT}/components/libraries/memobj"
    "${SDK_ROOT}/components/libraries/sha256"
    "${SDK_ROOT}/components/libraries/crc32"
    "${SDK_ROOT}/components/libraries/experimental_section_vars"
    "${SDK_ROOT}/components/libraries/mem_manager"
    "${SDK_ROOT}/components/libraries/fstorage"
    "${SDK_ROOT}/components/libraries/util"
    "${SDK_ROOT}/modules/nrfx"
    "${SDK_ROOT}/components/libraries/crypto/backend/oberon"
    "${SDK_ROOT}/components/libraries/crypto/backend/cifra"
    "${SDK_ROOT}/components/libraries/atomic"
    "${SDK_ROOT}/integration/nrfx"
    "${SDK_ROOT}/components/libraries/crypto/backend/cc310_bl"
    "${SDK_ROOT}/components/libraries/log/src"
    "${SDK_ROOT}/components/libraries/bootloader/dfu"
    "${SDK_ROOT}/components/ble/common"
    "${SDK_ROOT}/components/libraries/delay"
    "${SDK_ROOT}/components/softdevice/s112/headers/nrf52"
    "${SDK_ROOT}/components/libraries/svc"
    "${SDK_ROOT}/components/libraries/stack_info"
    "${SDK_ROOT}/components/libraries/crypto/backend/nrf_hw"
    "${SDK_ROOT}/components/libraries/log"
    "${SDK_ROOT}/components/libraries/strerror"
    "${SDK_ROOT}/components/libraries/crypto/backend/mbedtls"
    "${SDK_ROOT}/components/softdevice/s112/headers"
    "${SDK_ROOT}/components/boards"
    "${SDK_ROOT}/components/libraries/crypto/backend/cc310"
    "${SDK_ROOT}/components/libraries/bootloader"
    "${SDK_ROOT}/components/libraries/crypto"
    "${SDK_ROOT}/components/libraries/crypto/backend/optiga"
    "${SDK_ROOT}/components/libraries/scheduler"
    "${SDK_ROOT}/modules/nrfx/hal"
    "${SDK_ROOT}/components/toolchain/cmsis/include"
    "${SDK_ROOT}/components/libraries/balloc"
    "${SDK_ROOT}/components/libraries/atomic_fifo"
    "${SDK_ROOT}/components/libraries/crypto/backend/nrf_sw"
    "${SDK_ROOT}/modules/nrfx/mdk"
    "${SDK_ROOT}/components/libraries/bootloader/ble_dfu"
    "${SDK_ROOT}/components/softdevice/common"
    "${SDK_ROOT}/components/libraries/queue"
    "${SDK_ROOT}/components/libraries/ringbuf"
    CACHE INTERNAL ""
)

set(SDK_EXTERNAL_SOURCE_FILES
#    "${SDK_ROOT}/external/fprintf/nrf_fprintf.c"
#    "${SDK_ROOT}/external/fprintf/nrf_fprintf_format.c"
#    "${SDK_ROOT}/external/mbedtls/library/aes.c"
#    "${SDK_ROOT}/external/mbedtls/library/ctr_drbg.c"
    "${SDK_ROOT}/external/nano-pb/pb_common.c"
    "${SDK_ROOT}/external/nano-pb/pb_decode.c"
#    "${SDK_ROOT}/external/nano-pb/pb_encode.c"
#    "${SDK_ROOT}/external/utf_converter/utf.c"
#    "${SDK_ROOT}/external/segger_rtt/SEGGER_RTT.c"
#    "${SDK_ROOT}/external/segger_rtt/SEGGER_RTT_Syscalls_GCC.c"    #这里最好加判断分支用哪个原文件
#    "${SDK_ROOT}/external/segger_rtt/SEGGER_RTT_Syscalls_KEIL.c"
#    "${SDK_ROOT}/external/segger_rtt/SEGGER_RTT_printf.c"
    CACHE INTERNAL ""
)

set(SDK_EXTERNAL_INCLUDE_DIRS
    "${SDK_ROOT}/external/fprintf"
    "${SDK_ROOT}/external/mbedtls/include"
    "${SDK_ROOT}/external/nrf_cc310/include"
    "${SDK_ROOT}/external/nrf_oberon/include"
    "${SDK_ROOT}/external/nrf_tls/mbedtls/nrf_crypto/config"
    "${SDK_ROOT}/external/segger_rtt"
    "${SDK_ROOT}/external/utf_converter"
    "${SDK_ROOT}/external/nrf_oberon/include"
    "${SDK_ROOT}/external/nrf_oberon"
    "${SDK_ROOT}/external/micro-ecc/micro-ecc"
    "${SDK_ROOT}/external/nano-pb"
    CACHE INTERNAL ""
)

set(SDK_EXTERNAL_LIB_FILES          #第三方库
    "${SDK_ROOT}/external/nrf_oberon/lib/cortex-m4/soft-float/short-wchar/oberon_3.0.1.lib"
    "${SDK_ROOT}/external/micro-ecc/nrf52nf_keil/armgcc/micro_ecc_lib_nrf52.lib"
    CACHE INTERNAL ""
)


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
    -DFLOAT_ABI_SOFT)
endif()

target_compile_definitions(${target} PUBLIC
    ${USER_DEFINITIONS}
    -D_RTE_
    -DAPP_TIMER_V2
    -DAPP_TIMER_V2_RTC1_ENABLED
    -DBLE_STACK_SUPPORT_REQD
    -DBOARD_PCA10040
    -DNRFX_COREDEP_DELAY_US_LOOP_CYCLES=3
    -DBLE_LBS
    -DCONFIG_GPIO_AS_PINRESET
    -DNRF_DFU_SVCI_ENABLED
    -DDEVELOP_IN_NRF52832
    -DuECC_ENABLE_VLI_API=0
    -DuECC_OPTIMIZATION_LEVEL=3
    -DuECC_SQUARE_FUNC=0
    -DuECC_SUPPORT_COMPRESSED_POINT=0
    -DuECC_VLI_NATIVE_LITTLE_ENDIAN=1
    -DNRF_DFU_SETTINGS_VERSION=2
    -D__MICROLIB
    -DNRF52_PAN_74
    -DSVC_INTERFACE_CALL_AS_NORMAL_FUNCTION
    ${COMPILER_DEFS}            # -DFLOAT_ABI_SOFT
    ${${PLATFORM}_DEFINES}      # -DNRF52_SERIES;-DNRF52810;-DNRF52810_XXAA;-D__HEAP_SIZE=256;-D__STACK_SIZE=3072;-DNRF_MESH_LOG_ENABLE=0
    ${${SOFTDEVICE}_DEFINES})   # -DS112;-DSOFTDEVICE_PRESENT;-DNRF_SD_BLE_API_VERSION=7

message(STATUS "COMPILER_DEFS:${COMPILER_DEFS}")
message(STATUS "PLATFORM_DEFINES:${${PLATFORM}_DEFINES}")
message(STATUS "SOFTDEVICE_DEFINES:${${SOFTDEVICE}_DEFINES}")
message(STATUS "TOOLCHAIN:${TOOLCHAIN}")

target_link_libraries(${target}
    ${SDK_EXTERNAL_LIB_FILES}
)

create_hex(${target})
add_flash_target(${target})

