function(srEngine_link_icon TARGET)
    get_target_property(ICO_FOLDER ${TARGET} ICO_FOLDER)
    get_target_property(EXE ${TARGET} EXE)
    if (NOT ICO_FOLDER)
        return()
    endif()
    file(GLOB IMGS "${ICO_FOLDER}/*.png")
    if (NOT IMGS)
        message(AUTHOR_WARNING "GameSettings.json for ${TARGET} has speficied a folder for the icons, but the folder is empty. Skipping...")
        return()
    endif()

    set(OUTPUT_ICON "${CMAKE_CURRENT_BINARY_DIR}/app.ico")

    add_custom_target(
        ico_${TARGET}
        COMMENT "--- Generating app.ico..."
        COMMAND ${PYTHON_COMMAND}icon_generator.py
        ${CMAKE_CURRENT_BINARY_DIR}
        ${IMGS}
        DEPENDS ${IMGS}
        )
    
    add_dependencies(${TARGET} ico_${TARGET})

    if (WIN32)
        file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/ico.rc "IDI_ICON  ICON  DISCARDABLE  \"app.ico\"")
        target_sources(${EXE} PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/ico.rc)
    elseif(UNIX)
        configure_file(${SRENGINE_DIR}/asm/ico.s ${CMAKE_CURRENT_BINARY_DIR}/ico.s)
        target_link_libraries(${TARGET} PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/ico.s)
    endif()
endfunction()