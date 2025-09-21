function(srEngine_link_settings TARGET)
    set(PROJECT_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR})
    set(INPUT_SETTINGS ${PROJECT_SRC_DIR}/GameSettings.json)

    set(OUTPUT_FILE ${CMAKE_CURRENT_BINARY_DIR}/g_settings.cpp)

    macro(gen_settings INPUT_JSON)
        if (NOT ${INPUT_JSON} EQUAL 0)
            set(DEPEND DEPENDS ${INPUT_JSON})
        endif()

        message("-.- ${DEPEND}")
        add_custom_command(
            OUTPUT ${OUTPUT_FILE}
            ${DEPEND}
            COMMAND ${PYTHON_COMMAND}game_settings.py
            ${INPUT_JSON}
            ${CMAKE_CURRENT_BINARY_DIR}
            COMMENT "--- Generating game settings for ${TARGET}...")

        target_sources(${TARGET} PRIVATE ${OUTPUT_FILE})
    endmacro()

    if(EXISTS ${INPUT_SETTINGS})
        gen_settings(${INPUT_SETTINGS})
        file(READ ${INPUT_SETTINGS} JSON_CONT)
        string(JSON RE ERROR_VARIABLE "" GET ${JSON_CONT} "Resources")
        set_target_properties(${TARGET} PROPERTIES RES_FOLDER ${CMAKE_CURRENT_SOURCE_DIR}/${RE})
    else()
        message("--- Target ${TARGET} does not have GameSettings.json, using default")
        gen_settings(0)
    endif()
    
endfunction()