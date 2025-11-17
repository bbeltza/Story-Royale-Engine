function(srEngine_link_icon TARGET)
    get_target_property(ICO_FOLDER ${TARGET} ICO_FOLDER)
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
        ${OUTPUT_ICON}
        ${IMGS}
        DEPENDS ${IMGS}
        )
    
    add_dependencies(${TARGET} ico_${TARGET})

    if (WIN32)
        file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/ico.rc "IDI_ICON  ICON  DISCARDABLE  \"app.ico\"")
        target_sources(${TARGET} PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/ico.rc)
    elseif(UNIX)
        configure_file(${SRENGINE_DIR}/asm/ico.s.in ${OUTPUT}/ico.s)
        set_property(SOURCE ${OUTPUT}/ico.s PROPERTY LANGUAGE C)
        target_sources(${PROJECT} PUBLIC ${OUTPUT}/ico.s)
    endif()
endfunction()