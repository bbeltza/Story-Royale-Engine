find_package(Python REQUIRED)
if (NOT PYTHON_EXECUTABLE)
    set(PYTHON_EXECUTABLE python)
endif()
set(PYTHON_COMMAND ${PYTHON_EXECUTABLE} ${SRENGINE_DIR}/scripts/)

include(StoryRoyaleEngine/Resources)
include(StoryRoyaleEngine/Settings)
include(StoryRoyaleEngine/Icon)

function(srEngine_build TARGET EXE SRCS)
    project(${EXE})

    if (ANDROID)
        add_library(${TARGET} SHARED ${SOURCES})
    else()
        file(TOUCH "${CMAKE_BINARY_DIR}/empty.c")

        add_executable(${EXE} "${CMAKE_BINARY_DIR}/empty.c")
        add_library(${TARGET} STATIC ${${SRCS}})
        set_target_properties(${TARGET} PROPERTIES EXE ${EXE})
    endif()

    set_target_properties(${EXE} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${TARGET})
    set_target_properties(${TARGET} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${TARGET})
    if (MSVC_IDE)
        string(TOUPPER "RUNTIME_OUTPUT_DIRECTORY_${CMAKE_BUILD_TYPE}" RUNTIME_OUTPUT_VAR_MSVC)
        set_target_properties(${EXE} PROPERTIES ${RUNTIME_OUTPUT_VAR_MSVC} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${TARGET})
    endif()

    srEngine_link_settings(${TARGET} ${ARGN})
    srEngine_link_resource(${TARGET} ${ARGN})
    srEngine_link_icon(${TARGET})

    target_link_libraries(${TARGET} PUBLIC sre)
    target_link_libraries(${EXE} PUBLIC ${TARGET})
    target_include_directories(${TARGET} PUBLIC sre)

    if (ARGN MATCHES NO_CONSOLE)
        message("--- ${TARGET} HAS NO CONSOLE ${SR_ENGINE_NO_CONSOLE_OPTIONS}")
        target_link_libraries(${TARGET} PRIVATE sre_noconsole)
        endif()
        
    if (ARGN MATCHES USE_WX)
        message("--- ${TARGET} HAS WX")
        target_link_libraries(${TARGET} PRIVATE sre_werror)
    endif()

    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        target_link_options(${EXE} PRIVATE "-Wl,--start-group")
    endif()

    # Copying all of the dynamic libraries into the bin folder
    get_target_property(RUNTIME_OUTPUT_DIRECTORY ${EXE} RUNTIME_OUTPUT_DIRECTORY)
    file(GLOB DLLS_TO_COPY "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/*.so" "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/*.dll")
    file(INSTALL ${DLLS_TO_COPY} DESTINATION ${RUNTIME_OUTPUT_DIRECTORY})
endfunction()
