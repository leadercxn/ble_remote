find_program(NRFJPROG
    nrfjprog)

find_program(MERGEHEX
    mergehex)

if (NRFJPROG AND MERGEHEX)

    add_custom_target(merge)
    add_custom_target(flash)

    function(add_flash_target target)
        # Both the manual <merge> and <flash> target and depends on
        # the custom command that generates the merged hexfile.
        add_custom_target(merge_${target}
            DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${target}_merged.hex)

        add_dependencies(merge merge_${target})

        add_custom_target(flash_${target}
            COMMAND ${NRFJPROG} -f ${${PLATFORM}_FAMILY} --program ${CMAKE_CURRENT_BINARY_DIR}/${target}_merged.hex --sectorerase --reset
            DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${target}_merged.hex)

        add_dependencies(flash flash_${target})

        add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${target}_merged.hex
            COMMAND ${MERGEHEX} -m ${${SOFTDEVICE}_HEX_FILE} ${CMAKE_CURRENT_BINARY_DIR}/${target}.hex -o ${CMAKE_CURRENT_BINARY_DIR}/${target}_merged.hex
            DEPENDS ${target}
            VERBATIM)
    endfunction(add_flash_target)


    add_custom_target(erase
        COMMAND ${NRFJPROG} -e)

    add_custom_target(sd
        COMMAND ${NRFJPROG} --program ${${SOFTDEVICE}_HEX_FILE} -f ${${PLATFORM}_FAMILY} --chiperase)

    add_custom_target(flashfac          
        COMMAND ${NRFJPROG} --program ${CMAKE_BINARY_DIR}/apps/factory_test/factory.hex -f ${${PLATFORM}_FAMILY} --sectorerase --reset)

    add_custom_target(flashapp
        COMMAND ${NRFJPROG} --program ${CMAKE_BINARY_DIR}/apps/app/app.hex -f ${${PLATFORM}_FAMILY} --sectorerase --reset)

     add_custom_target(flashdfu
        COMMAND ${NRFJPROG} --program ${CMAKE_BINARY_DIR}/apps/dfu/dfu.hex -f ${${PLATFORM}_FAMILY} --sectorerase --reset)

else ()
    message(STATUS "Could not find nRFx command line tools (`nrfjprog` and `mergehex`).
   See http://infocenter.nordicsemi.com/topic/com.nordic.infocenter.tools/dita/tools/nrf5x_command_line_tools/nrf5x_installation.html?cp=5_1_1.
   Flash target will not be supported.")
    function(add_flash_target target)
        # Not supported
    endfunction(add_flash_target)
endif (NRFJPROG AND MERGEHEX)