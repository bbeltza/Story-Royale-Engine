include(sre/python)

function(srEngine_link_icon TARGET)
    get_target_property(ICO_FOLDER ${TARGET} ICO_FOLDER)
    if (NOT ICO_FOLDER)
        return()
    endif()

    if (NOT EXISTS ${ICO_FOLDER})
        message(FATAL_ERROR "-- Could not find icon folder ${ICO_FOLDER} for '${TARGET}'. Path doesn't exist or is not a directory")
    endif()
    file(GLOB IMGS "${ICO_FOLDER}/*.png")
    if (NOT IMGS)
        message(AUTHOR_WARNING "${TARGET}'s properties have speficied a folder for the icons, but the folder is empty. Skipping...")
        return()
    endif()

    set(OUTPUT_GROUP "${CMAKE_CURRENT_BINARY_DIR}/icons")

    # Make this for windows only
    add_custom_target(
        icowin32_${TARGET}
        COMMENT "--- Generating app.ico..."
        COMMAND ${PYTHON_COMMAND}win32icon.py
        ${CMAKE_CURRENT_BINARY_DIR}
        ${IMGS}
        DEPENDS ${IMGS}
     )

     add_custom_target(
         icogroup_${TARGET}
         COMMENT "--- Grouping icons..."
         COMMAND ${PYTHON_COMMAND}icon_grouper.py
         ${OUTPUT_GROUP}
         ${IMGS}
         DEPENDS ${IMGS}
     )    

    if (WIN32)
        add_dependencies(${TARGET} icowin32_${TARGET})
        file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/ico.rc "IDI_ICON1  ICON  DISCARDABLE  \"app.ico\"")
        target_sources(${TARGET} PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/ico.rc)
    else()
        add_dependencies(${TARGET} icogroup_${TARGET})
        find_program(HAVE_XDG xdg-desktop-menu)
        if (HAVE_XDG)
            foreach(X 16 24 32 48 64 128)
            if (EXISTS ${OUTPUT_GROUP}/${X}x${X})
                    set(ICON_DIR "~/.local/share/icons/hicolor/${X}x${X}/apps/")
                    install(FILES ${OUTPUT_GROUP}/${X}x${X} DESTINATION ${ICON_DIR} RENAME ${TARGET}.png)
                endif()
            endforeach()
        endif()
    endif()
endfunction()