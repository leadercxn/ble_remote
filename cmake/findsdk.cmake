set(nRF5_SDK_VERSION "nRF5_SDK_16.0.0_98a08e2" CACHE STRING "nRF5 SDK")

if (NOT nRF5_SDK_VERSION)
    message(FATAL_ERROR "You need to specifiy a nRF5_SDK_VERSION to use.")
endif()

set(DEFAULT_SDK_ROOT "${CMAKE_SOURCE_DIR}/../${nRF5_SDK_VERSION}")
find_dependency(SDK_ROOT
    "Path to nRF5 SDK root"
    "${DEFAULT_SDK_ROOT}"
    "license.txt")

if(NOT SDK_ROOT)
    message(WARNING "
    Could not find the nRF5 SDK. The build will fail.
    Please run the nRF5_SDK target to download it or provide the correct path
    using the -DSDK_ROOT option or setting the SDK_ROOT environment variable.
    After the download is complete, re-run `cmake`.\n")
endif()