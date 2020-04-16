set(PLATFORM "nrf52810_xxAA"
    CACHE STRING "Choose the target platform to build for. Use \"host\" for unit test builds.")
set_property(CACHE PLATFORM PROPERTY STRINGS
    "nrf52833_xxAA" "nrf52832_xxAA" "nrf52840_xxAA" "nrf52810_xxAA")

if (NOT EXISTS "${CMAKE_CONFIG_DIR}/platform/${PLATFORM}.cmake")
    message(FATAL_ERROR "Platform specific file for ${PLATFORM} not found.")
endif()