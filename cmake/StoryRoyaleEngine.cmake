find_package(Python REQUIRED)
set(PYTHON_COMMAND COMMAND ${PYTHON_EXECUTABLE} ${SRENGINE_DIR}/scripts/)

macro(srEngine_policy)
    cmake_policy(SET CMP0079 NEW)
    cmake_policy(SET CMP0091 NEW)
    cmake_policy(SET CMP0025 NEW)
endmacro()

macro(srEngine_include_directories target)
    target_include_directories(${target} PUBLIC StoryRoyaleEngine)
endmacro()

function(srEngine_link_resource PROJECT)
    get_target_property(INPUT ${PROJECT} INPUT)
    get_target_property(BIND_RESOURCES ${PROJECT} WANT_BINDING)
    get_target_property(SRC ${PROJECT} SRC)
    

    if(NOT INPUT)
    return()
    endif()


    if (WANT_BINDING)
        set(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}")
        add_custom_target(
        bind_${PROJECT}
        COMMENT "--- Binding source resources..."
        ${PYTHON_COMMAND}resource_binder.py
        ${INPUT} 1
        ${OUTPUT}
        OUTPUT ${OUTPUT}/_res.c
        DEPENDS ${INPUT}
        )

        file(WRITE ${OUTPUT}/_res.c)
        
        add_dependencies(${PROJECT} bind_${PROJECT})

        target_sources(${PROJECT} PRIVATE ${OUTPUT}/_res.c)
    else()
        get_target_property(OUTPUT ${PROJECT} RUNTIME_OUTPUT_DIRECTORY)
        add_custom_target(
            copy_${PROJECT}
            COMMENT "--- Copying source resources into ${OUTPUT}/${SRC}"
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${INPUT}
            ${OUTPUT}/${SRC}
            )
        add_custom_target(
            rename_${PROJECT}
            COMMENT "--- Renaming resources folder to '_res'"
            COMMAND ${CMAKE_COMMAND} -E rm -rf ${OUTPUT}/_res
            COMMAND ${CMAKE_COMMAND} -E rename
            ${OUTPUT}/${SRC} ${OUTPUT}/_res
        )
        message("-- ${OUTPUT}")
        message("-- ${CMAKE_BUILD_TYPE}")
        message("${CMAKE_CURRENT_BINARY_DIR}/${SRC}")
        add_dependencies(${PROJECT} copy_${PROJECT})
        add_dependencies(${PROJECT} rename_${PROJECT})
    endif()
endfunction()

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
            ${PYTHON_COMMAND}game_settings.py
            ${INPUT_JSON}
            ${CMAKE_CURRENT_BINARY_DIR}
            COMMENT "--- Generating game settings for ${TARGET}...")

        target_sources(${TARGET} PRIVATE ${OUTPUT_FILE})
    endmacro()

    if(EXISTS ${INPUT_SETTINGS})
        gen_settings(${INPUT_SETTINGS})
    else()
        message("--- Target ${TARGET} does not have GameSettings.json, using default")
        gen_settings(0)
    endif()
    
endfunction()

function(srEngine_link_target target)
    set_target_properties(${target} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${target})
    set_target_properties(${target} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${target})
    get_target_property(BIND_RESOURCES ${target} WANT_BINDING)

    target_link_libraries(${target} StoryRoyaleEngine)
    srEngine_link_resource(${target})
    srEngine_link_settings(${target})
    
    if (${ARGN} MATCHES NO_CONSOLE)
        target_link_options(${target} PRIVATE ${SRENGINE_NO_CONSOLE_OPTIONS})
    endif()

    if (NOT BIND_RESOURCES)
        message("--- Linking ${target} with a no-binder")
        target_link_libraries(${target} no_bind)
    endif()
endfunction()