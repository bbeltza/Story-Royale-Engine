function(srEngine_link_settings TARGET)
    get_target_property(PROJECT_SRC_DIR ${TARGET} SOURCE_DIR)
    set(INPUT_SETTINGS ${PROJECT_SRC_DIR}/GameSettings.json)
    set(OUTPUT_FILE ${CMAKE_CURRENT_BINARY_DIR}/g_settings.c)

    macro(gen_settings INPUT_JSON)
        if (NOT ${INPUT_JSON} EQUAL 0)
            set(DEPEND DEPENDS ${INPUT_JSON})
        endif()

        message("--- ${DEPEND}")
        add_custom_command(
            OUTPUT ${OUTPUT_FILE}
            ${DEPEND}
            COMMAND ${PYTHON_COMMAND}game_settings.py
            ${INPUT_JSON}
            ${CMAKE_CURRENT_BINARY_DIR}
            COMMENT "--- Generating game settings for ${TARGET}...")

        target_sources(${TARGET} PUBLIC ${OUTPUT_FILE})
    endmacro()

    if(EXISTS ${INPUT_SETTINGS})
        gen_settings(${INPUT_SETTINGS})
        file(READ ${INPUT_SETTINGS} JSON_CONT)
        string(JSON RE ERROR_VARIABLE "" GET ${JSON_CONT} "Resources")
        string(JSON IC ERROR_VARIABLE "" GET ${JSON_CONT} "Icon")
        string(JSON TT ERROR_VARIABLE "" GET ${JSON_CONT} "Title")
        if(RE)
            set_target_properties(${TARGET} PROPERTIES RES_FOLDER ${PROJECT_SRC_DIR}/${RE})
        endif()
        if(IC)
            set_target_properties(${TARGET} PROPERTIES ICO_FOLDER ${PROJECT_SRC_DIR}/${IC})
        endif()
        if(TT)
            set_target_properties(${TARGET} PROPERTIES TITLE_STRING ${TT})
        endif()
    else()
        message("--- Target ${TARGET} does not have GameSettings.json, using default")
        gen_settings(0)
    endif()
    
endfunction()