find_package(Python REQUIRED)
set(PYTHON_COMMAND python ${SRENGINE_DIR}/scripts/)

include(StoryRoyaleEngine/Policy)
include(StoryRoyaleEngine/Resources)
include(StoryRoyaleEngine/Settings)

function(srEngine_build TARGET)
    add_executable(${TARGET} ${SOURCES})
    
    set_target_properties(${TARGET} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${TARGET})
    set_target_properties(${TARGET} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${TARGET})

    srEngine_link_settings(${TARGET} ${ARGN})
    srEngine_link_resource(${TARGET} ${ARGN})

    target_include_directories(${TARGET} PRIVATE StoryRoyaleEngine)
    target_link_libraries(${TARGET} PRIVATE StoryRoyaleEngine)

    if ("${ARGN}" MATCHES NO_CONSOLE)
        message("--- ${TARGET} HAS NO CONSOLE ${SR_ENGINE_NO_CONSOLE_OPTIONS}")
        target_link_options(${TARGET} PRIVATE ${SR_ENGINE_NO_CONSOLE_OPTIONS})
    endif()
endfunction()
