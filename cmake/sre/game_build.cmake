include(sre/game_icon)
include(sre/game_property)
include(sre/game_resources)

option(SRE_GAME_MINIMAL "Build binaries only, without building the rest (Resource folders, icons, etc...)" OFF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/lib)

if (UNIX) # Enable assembly to .incbin embedded resources
    enable_language(ASM)
endif()

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
    
    macro(SETIFEXISTS VAR PROP VALUE)
        if (${VAR})
            set_target_properties(${TARGET_EXE} PROPERTIES ${PROP} ${VALUE})
        endif()
    endmacro()

    SETIFEXISTS(GAME_RESOURCES RES_FOLDER ${CMAKE_CURRENT_SOURCE_DIR}/${GAME_RESOURCES})
    SETIFEXISTS(GAME_ICONS ICO_FOLDER ${CMAKE_CURRENT_SOURCE_DIR}/${GAME_ICONS})

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
endfunction()