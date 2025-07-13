find_package(PythonInterp)
find_package(Python)

macro(srEngine_set_resource_file PROJECT SRC)
    set(INPUT "${CMAKE_CURRENT_SOURCE_DIR}/${SRC}/")
    set(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}")
    message("-- ${CMAKE_CURRENT_BINARY_DIR}/")

    if (PYTHON_FOUND)
        add_custom_target(
        copy_${PROJECT} ALL
        COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_SOURCE_DIR}/scripts/resource_binder.py
        ${INPUT} 1
        ${OUTPUT}
        COMMENT "--- Binding source resources"
        )

        file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/_res.c)

        project(${PROJECT}_res)
        add_library(${PROJECT}_res ${CMAKE_CURRENT_BINARY_DIR}/_res.c)
        target_link_libraries(${PROJECT} ${PROJECT}_res)
    else()
        add_custom_target(
            copy_${PROJECT} ALL
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${INPUT}
            ${OUTPUT}/${CMAKE_BUILD_TYPE}/${SRC}
            COMMENT "--- Copying source resources"
            )
        add_custom_target(
            rename_${PROJECT} ALL
            COMMAND ${CMAKE_COMMAND} -E rm -rf ${OUTPUT}/${CMAKE_BUILD_TYPE}/_res
            COMMAND ${CMAKE_COMMAND} -E rename
            ${OUTPUT}/${CMAKE_BUILD_TYPE}/${SRC} ${OUTPUT}/${CMAKE_BUILD_TYPE}/_res
            COMMENT "--- Renaming resources folder to 'res'"
        )
        message("-- ${OUTPUT}")
        message("-- ${CMAKE_BUILD_TYPE}")
        message("${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/${SRC}")
    endif()
        add_dependencies(${PROJECT} copy_${PROJECT})
        if (TARGET rename_${PROJECT})
            add_dependencies(${PROJECT} rename_${PROJECT})
        endif()
endmacro()
