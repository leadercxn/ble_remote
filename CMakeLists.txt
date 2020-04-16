cmake_minimum_required(VERSION 3.14)

message(STATUS "Configuring CMake for nRF5 SDK")

set(CMAKE_SYSTEM_NAME "Generic")
set(CMAKE_SYSTEM_PROCESSOR "ARM")

project(nrf52_template LANGUAGES NONE)

set(CMAKE_CONFIG_DIR "${CMAKE_SOURCE_DIR}/cmake")
set(CMAKE_SOURCE_DIR ${CMAKE_SOURCE_DIR} CACHE STRING "" FORCE)

add_custom_target(pristine
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CONFIG_DIR}/pristine.cmake
# Equivalent to rm -rf build/*
)

include("${CMAKE_CONFIG_DIR}/toolchain.cmake")
include("${CMAKE_CONFIG_DIR}/platform.cmake")
include("${CMAKE_CONFIG_DIR}/softdevice.cmake")
include("${CMAKE_CONFIG_DIR}/finddependency.cmake")
include("${CMAKE_CONFIG_DIR}/findsdk.cmake")

# We have to return manually from here s.t. the CMake generation
# doesn't stop and we have the nRF5_SDK target available.
if (NOT SDK_ROOT)
    return()
endif ()

include("${CMAKE_CONFIG_DIR}/sdk/${nRF5_SDK_VERSION}.cmake")
include("${CMAKE_CONFIG_DIR}/platform/${PLATFORM}.cmake")
include("${CMAKE_CONFIG_DIR}/softdevice/${SOFTDEVICE}.cmake")

message(STATUS "SDK: ${nRF5_SDK_VERSION}")
message(STATUS "Platform: ${PLATFORM}")
message(STATUS "Arch: ${${PLATFORM}_ARCH}")
message(STATUS "SoftDevice: ${SOFTDEVICE}")


set(ARCH ${${PLATFORM}_ARCH})

enable_language(C ASM)

set(CMAKE_EXECUTABLE_SUFFIX ".elf")
set(BUILD_SHARED_LIBS OFF)
set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "")

include("${CMAKE_CONFIG_DIR}/nrfjprog.cmake")

add_subdirectory(apps/app)
add_subdirectory(apps/factory_test)
add_subdirectory(apps/dfu)




