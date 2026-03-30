find_package(Python REQUIRED)
set(PYTHON_COMMAND ${Python_EXECUTABLE} ${SRE_DIR}/scripts/)

include(sre/Resources)
include(sre/Icon)

# A couple of helpers

macro(SRE_PROPERTY_TARGET TARGET VAR)
    set(${VAR} ${TARGET}_PROPERTY_INTERFACE)
endmacro()

#

function(SRE_PROPERTY TARGET)
    SRE_PROPERTY_TARGET(${TARGET} INT)
    if (NOT TARGET ${INT})
        project(${INT})
        add_library(${INT} INTERFACE)
    endif()

    set_target_properties(${INT} ${ARGN})
endfunction()

function(SRE_BUILD TARGET_EXE SRCS)
    if (NOT ${SRCS})
        message(FATAL_ERROR "No sources speficied in variable '${SRCS}'. Note that you won't be able to compile a game if you don't define 'sre::initailize()'!")
    endif()

    project(${TARGET_EXE})

    # Setup game code
    set(SOURCES "${CMAKE_CURRENT_BINARY_DIR}/title.c" ${${SRCS}})
    if (ANDROID)
        add_library(${TARGET_EXE} SHARED ${SOURCES})
    else ()
        add_executable(${TARGET_EXE} ${SOURCES})
    endif()
    #

    # Get the set properties, if they've been defined
    SRE_PROPERTY_TARGET(${TARGET_EXE} INT)
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
    
    macro(SETIFEXISTS) endmacro()
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

    target_link_libraries(${TARGET_EXE} PUBLIC srelib)

    if (ARGN MATCHES NO_CONSOLE)
        message("--- ${TARGET} HAS NO CONSOLE ${SR_ENGINE_NO_CONSOLE_OPTIONS}")
        target_link_libraries(${TARGET_EXE} PRIVATE sre_noconsole)
    endif()
    if (ARGN MATCHES USE_WX)
        message("--- ${TARGET} HAS WX")
        target_link_libraries(${TARGET_EXE} PRIVATE sre_werror)
    endif()

    if(NOT MSVC) # Incremental linking, this is needed for non-MSVC compilers
        target_link_options(${TARGET_EXE} PRIVATE "-Wl,--start-group")
    endif()
    if (ANDROID) # Support for 16kb aligned memory pages (Support for disabling the option might come)
        target_link_options(${TARGET_EXE} PRIVATE "-Wl,-z,max-page-size=16384")
    endif()
endfunction()

# Install files to the executable folder
function(SRE_INSTALL_TOBIN TARGET)
    if (NOT ARGN)
        message(WARN "Call to SRE_INSTALL_TOBIN() without any file arguments!")
        return()
    endif()

    get_target_property(OUT ${TARGET_EXE} RUNTIME_OUTPUT_DIRECTORY)
    foreach(DIR ${ARGN})
        set(SODIR "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${DIR}${CMAKE_SHARED_LIBRARY_SUFFIX}")
        if (EXISTS ${SODIR})
            file(INSTALL ${SODIR} DESTINATION ${OUT})
        endif()
        file(INSTALL ${DIR} DESTINATION ${OUT})
    endforeach()
endfunction()