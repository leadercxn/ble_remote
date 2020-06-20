find_program(NRFJPROG   nrfjprog)
find_program(MERGEHEX   mergehex)
find_program(NRFUTIL    nrfutil)


if(NOT NRFJPROG)
    message(FATAL_ERROR "Tool nrfjprog: not found")
endif()

if(NOT MERGEHEX)
    message(FATAL_ERROR "Tool mergehex: not found")
endif()

if(NOT NRFUTIL)
    message(FATAL_ERROR "Tool nrfutil: not found")
endif()


if (NRFJPROG AND MERGEHEX AND NRFUTIL)

    add_custom_target(erase
        COMMAND ${NRFJPROG} -e)

    add_custom_target(sd
        COMMAND ${NRFJPROG} --program ${${SOFTDEVICE}_HEX_FILE} -f ${${PLATFORM}_FAMILY} --chiperase)

    add_custom_target(bl
        COMMAND ${NRFJPROG} --program ${SDK_BOOTLOADER_HEX_FILE} -f ${${PLATFORM}_FAMILY} --chiperase)

    add_custom_target(merge
        COMMAND ${NRFUTIL} settings generate --family NRF52810 --application ${CMAKE_BINARY_DIR}/apps/app/app.hex --application-version 1 --bootloader-version 1 --bl-settings-version 1 ${CMAKE_BINARY_DIR}/apps/app/bootloader_settings.hex
        COMMAND ${MERGEHEX}  -m  ${${SOFTDEVICE}_HEX_FILE}  ${CMAKE_BINARY_DIR}/apps/app/app.hex  ${SDK_BOOTLOADER_HEX_FILE}  -o  ${CMAKE_BINARY_DIR}/apps/app/sd_app_bl.hex 
        #COMMAND ${MERGEHEX}  -m  ${CMAKE_BINARY_DIR}/apps/app/sd_app_bl.hex  ${CMAKE_BINARY_DIR}/apps/app/bootloader_settings.hex  -o  ${CMAKE_BINARY_DIR}/apps/app/sd_app_bl_merge.hex
        COMMAND ${MERGEHEX}  -m  ${CMAKE_BINARY_DIR}/apps/app/sd_app_bl.hex  ${CMAKE_BINARY_DIR}/apps/app/bootloader_settings.hex  -o  ${CMAKE_BINARY_DIR}/../firmware/SENSORO_SSS_CONTROLLER_1010_FW_${${target}_VERSION}_${BUILD_HASH}_${BUILD_DATE}.hex 
        COMMAND rm               ${CMAKE_BINARY_DIR}/apps/app/sd_app_bl.hex
        COMMAND rm               ${CMAKE_BINARY_DIR}/apps/app/bootloader_settings.hex)

    #带变量的命令不能放在function外面，传参不了
    #add_custom_target(flash_${target}              
    #        COMMAND ${NRFJPROG} --program ${CMAKE_CURRENT_BINARY_DIR}/${target}.hex -f ${${PLATFORM}_FAMILY} --sectorerase --reset
    #        DEPENDS ${target})

    function(add_flash_target target)
        # Flash target command
        add_custom_target(flash_${target}           #烧录app
            COMMAND ${NRFJPROG} --program ${CMAKE_CURRENT_BINARY_DIR}/${target}.hex -f ${${PLATFORM}_FAMILY} --sectorerase --reset
            DEPENDS ${target})

        add_custom_target(flash_sd_${target}        #烧录sd、app
            COMMAND ${NRFJPROG} --program ${CMAKE_CURRENT_BINARY_DIR}/${target}.hex -f ${${PLATFORM}_FAMILY} --sectorerase --reset
            DEPENDS sd ${target})           #先执行 目标为 sd 的命令
    endfunction(add_flash_target)

    add_custom_target(dfu_pkg  
        COMMAND ${NRFUTIL} pkg generate --hw-version 52 --sd-req 0xCD --application-version 2 --application ${CMAKE_BINARY_DIR}/apps/app/app.hex  --key-file  ${KEY_FILE}   ${CMAKE_BINARY_DIR}/../firmware/SENSORO_SSS_CONTROLLER_1010_DFU_${${target}_VERSION}_${BUILD_HASH}_${BUILD_DATE}.zip )

    add_custom_target(flash_merge    
        COMMAND ${NRFJPROG} --program ${CMAKE_BINARY_DIR}/apps/app/sd_app_bl_merge.hex -f ${${PLATFORM}_FAMILY} --sectorerase --reset)

    add_custom_target(flashfac          
        COMMAND ${NRFJPROG} --program ${CMAKE_BINARY_DIR}/apps/factory_test/factory.hex -f ${${PLATFORM}_FAMILY} --sectorerase --reset)

    add_custom_target(flashapp
        COMMAND ${NRFJPROG} --program ${CMAKE_BINARY_DIR}/apps/app/app.hex -f ${${PLATFORM}_FAMILY} --sectorerase --reset)

    add_custom_target(sd_app
        COMMAND ${MERGEHEX}  -m  ${${SOFTDEVICE}_HEX_FILE}  ${CMAKE_BINARY_DIR}/apps/app/app.hex  -o  ${CMAKE_BINARY_DIR}/apps/app/sd_app_merge.hex )

    add_custom_target(flashsdapp
        COMMAND ${NRFJPROG} --program ${CMAKE_BINARY_DIR}/apps/app/sd_app_merge.hex -f ${${PLATFORM}_FAMILY} --sectorerase --reset)

else ()
    message(STATUS "Could not find nRFx command line tools (`nrfjprog` and `mergehex`).
   See http://infocenter.nordicsemi.com/topic/com.nordic.infocenter.tools/dita/tools/nrf5x_command_line_tools/nrf5x_installation.html?cp=5_1_1.
   Flash target will not be supported.")
    function(add_flash_target target)
        # Not supported
    endfunction(add_flash_target)
endif (NRFJPROG AND MERGEHEX AND NRFUTIL)