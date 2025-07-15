macro(srEngine_policy)

    cmake_policy(SET CMP0079 NEW)
    cmake_policy(SET CMP0091 NEW)

endmacro()

function(srEngine_include_directories target)
    if(!SRENGINE_INCLUDE_DIRS)
        message(SEND_ERROR "ERROR: No include directories found")
    endif()
    foreach(include_dir ${SRENGINE_INCLUDE_DIRS})
        target_include_directories(${target} PRIVATE ${include_dir})
    endforeach()
endfunction()

function(srEngine_link_resource PROJECT)
    get_target_property(INPUT ${PROJECT} INPUT)
    get_target_property(BIND_RESOURCES ${PROJECT} WANT_BINDING)
    get_target_property(SRC ${PROJECT} SRC)
    set(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}")
    message("-- ${CMAKE_CURRENT_BINARY_DIR}/")
    message("-- ${PROJECT} ${WANT_BINDING}")

    if(NOT INPUT)
    return()
    endif()


    if (PYTHON_FOUND AND WANT_BINDING)
        add_custom_target(
        bind_${PROJECT} ALL
        COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_SOURCE_DIR}/scripts/resource_binder.py
        ${INPUT} 1
        ${OUTPUT}
        OUTPUT ${OUTPUT}/_res.c
        COMMENT "--- Binding source resources"
        )

        file(WRITE ${OUTPUT}/_res.c)
        
        add_dependencies(${PROJECT} bind_${PROJECT})

        project(${PROJECT}_res)
        add_library(${PROJECT}_res ${CMAKE_CURRENT_BINARY_DIR}/_res.c)
        target_link_libraries(${PROJECT} ${PROJECT}_res)
    else()
        add_custom_target(
            copy_${PROJECT} ALL
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${INPUT}
            ${OUTPUT}/${CMAKE_BUILD_TYPE}/${SRC}
            COMMENT "--- Copying source resources into ${OUTPUT}/${CMAKE_BUILD_TYPE}/${SRC}"
            )
        add_custom_target(
            rename_${PROJECT} ALL
            COMMAND ${CMAKE_COMMAND} -E rm -rf ${OUTPUT}/${CMAKE_BUILD_TYPE}/_res
            COMMAND ${CMAKE_COMMAND} -E rename
            ${OUTPUT}/${CMAKE_BUILD_TYPE}/${SRC} ${OUTPUT}/${CMAKE_BUILD_TYPE}/_res
            COMMENT "--- Renaming resources folder to '_res'"
        )
        message("-- ${OUTPUT}")
        message("-- ${CMAKE_BUILD_TYPE}")
        message("${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/${SRC}")
        add_dependencies(${PROJECT} copy_${PROJECT})
        add_dependencies(${PROJECT} rename_${PROJECT})
    endif()
endfunction()

function(srEngine_link_target target)
    if (${ARGN} MATCHES NO_CONSOLE)
        target_link_options(${target} PRIVATE ${NO_CONSOLE_OPTIONS})
    endif()
    target_link_libraries(${target} StoryRoyaleEngine)
    srEngine_link_resource(${target})
    if (NOT TARGET ${target}_res)
        message("NOT BINDING RESOURCES")
        target_link_libraries(${target} no_bind)
    endif()
endfunction()