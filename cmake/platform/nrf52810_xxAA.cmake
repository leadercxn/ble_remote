set(nrf52810_xxAA_ARCH "cortex-m4")
set(nrf52810_xxAA_SOURCE_FILES
    "${SDK_ROOT}/modules/nrfx/mdk/system_nrf52810.c")

set(nrf52810_xxAA_INCLUDE_DIRS
    "${SDK_ROOT}/modules/nrfx"
    "${SDK_ROOT}/modules/nrfx/mdk"
    "${SDK_ROOT}/modules/nrfx/hal"
    "${SDK_ROOT}/modules/nrfx/templates/nRF52810"
    "${SDK_ROOT}/components/toolchain/cmsis/include")

if (TOOLCHAIN MATCHES "armgcc" OR TOOLCHAIN STREQUAL "clang")
    set(nrf52810_xxAA_SOURCE_FILES
        ${nrf52810_xxAA_SOURCE_FILES}
        "${SDK_ROOT}/modules/nrfx/mdk/gcc_startup_nrf52810.S")
    set(nrf52810_xxAA_INCLUDE_DIRS
        ${nrf52810_xxAA_INCLUDE_DIRS}
        "${SDK_ROOT}/components/toolchain/gcc"
        "${SDK_ROOT}/components/toolchain/cmsis/dsp/GCC")
    set(nrf52810_xxAA_LINK_INCLUDE_DIR
        "${SDK_ROOT}/modules/nrfx/mdk")
elseif (TOOLCHAIN STREQUAL "armcc")
    set(nrf52810_xxAA_SOURCE_FILES
        ${nrf52810_xxAA_SOURCE_FILES}
        "${SDK_ROOT}/modules/nrfx/mdk/arm_startup_nrf52810.s")
    set(nrf52810_xxAA_INCLUDE_DIRS
        ${nrf52810_xxAA_INCLUDE_DIRS}
        "${SDK_ROOT}/components/toolchain/cmsis/dsp/ARM")
else ()
    message(FATAL_ERROR "Unknown toolchain ${TOOLCHAIN}")
endif ()

set(nrf52810_xxAA_DEFINES
    -DNRF52_SERIES
    -DNRF52810
    -DNRF52810_XXAA
    #-DNRF52_PAN_74
    -D__STACK_SIZE=2048
    # TODO: __HEAP_SIZE is less than the 384 byte maximum for segmented messages.
    # I.e., to use full length segmented messages you would have to increase this value.
    -D__HEAP_SIZE=0
    -DNRF_MESH_LOG_ENABLE=0)

set(nrf52810_xxAA_FAMILY "NRF52")

#set(CMAKE_BUILD_TYPE "DEBUG")

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(WARNING
        "The program might be too large for the nRF52810 in Debug mode. "
        "Using CMAKE_BUILD_TYPE=MinSizeRel is recommended.")
endif (CMAKE_BUILD_TYPE STREQUAL "Debug")
