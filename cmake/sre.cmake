find_package(Python REQUIRED)
set(PYTHON_COMMAND ${Python_EXECUTABLE} ${SRE_DIR}/scripts/)

include(sre/Resources)
include(sre/Icon)

function(srEngine_property TARGET)
    set(INT ${TARGET}_PROPERTY_INTERFACE)
    project(${INT})
    add_library(${INT} INTERFACE)

    set_target_properties(${INT} ${ARGN})
endfunction()

function(srEngine_build TARGET_EXE SRCS)
    project(${TARGET_EXE})
    
    if (ANDROID)
        add_library(${TARGET_EXE} SHARED "${CMAKE_CURRENT_BINARY_DIR}/title.c" ${${SRCS}})
    else ()
        add_executable(${TARGET_EXE} "${CMAKE_CURRENT_BINARY_DIR}/title.c" ${${SRCS}})
    endif()

    set(INT ${TARGET_EXE}_PROPERTY_INTERFACE)
    if (TARGET ${INT})
        get_target_property(GAME_TITLE ${INT} TITLE)
        get_target_property(GAME_ICONS ${INT} ICONS)
        get_target_property(GAME_RESOURCES ${INT} RESOURCES)
        target_link_libraries(${TARGET_EXE} PUBLIC ${INT})
    endif()

    if(NOT GAME_TITLE)
        set(GAME_TITLE ${TARGET_EXE}) # Set the title to the TARGET name if no title property is set
    endif()
    configure_file(${SRE_DIR}/gen_src/title.c.in ${CMAKE_CURRENT_BINARY_DIR}/title.c)
    
    if (GAME_RESOURCES)
        set_target_properties(${TARGET_EXE} PROPERTIES RES_FOLDER ${CMAKE_CURRENT_SOURCE_DIR}/${GAME_RESOURCES})
    endif()
    if (GAME_ICONS)
        set_target_properties(${TARGET_EXE} PROPERTIES ICO_FOLDER ${CMAKE_CURRENT_SOURCE_DIR}/${GAME_ICONS})
    endif()

    set_target_properties(${TARGET_EXE} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${TARGET_EXE})
    if (MSVC_IDE)
        string(TOUPPER "RUNTIME_OUTPUT_DIRECTORY_${CMAKE_BUILD_TYPE}" RUNTIME_OUTPUT_VAR_MSVC)
        set_target_properties(${TARGET_EXE} PROPERTIES ${RUNTIME_OUTPUT_VAR_MSVC} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${TARGET_EXE})
    endif()

    srEngine_link_resource(${TARGET_EXE} ${ARGN})
    srEngine_link_icon(${TARGET_EXE})

    target_link_libraries(${TARGET_EXE} PUBLIC sre)

    if (ARGN MATCHES NO_CONSOLE)
        message("--- ${TARGET} HAS NO CONSOLE ${SR_ENGINE_NO_CONSOLE_OPTIONS}")
        target_link_libraries(${TARGET_EXE} PRIVATE sre_noconsole)
    endif()
        
    if (ARGN MATCHES USE_WX)
        message("--- ${TARGET} HAS WX")
        target_link_libraries(${TARGET_EXE} PRIVATE sre_werror)
    endif()

    if(NOT MSVC)
        target_link_options(${TARGET_EXE} PRIVATE "-Wl,--start-group")
    endif()

    # Copying all of the dynamic libraries into the bin folder
    if (NOT ANDROID)
        get_target_property(RUNTIME_OUTPUT_DIRECTORY ${TARGET_EXE} RUNTIME_OUTPUT_DIRECTORY)
        file(GLOB DLLS_TO_COPY "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/*.so" "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/*.dll")
        file(INSTALL ${DLLS_TO_COPY} DESTINATION ${RUNTIME_OUTPUT_DIRECTORY})
    endif()
endfunction()
