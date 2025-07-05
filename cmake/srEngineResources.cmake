macro(srEngine_set_resource_file PROJECT SRC)
    message("-- " ${PROJECT})
    message("-- " ${CMAKE_CURRENT_BINARY_DIR}/${SRC})
    add_custom_target(
        copy ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_LIST_DIR}/${SRC}/
        ${CMAKE_CURRENT_BINARY_DIR}/${SRC}
        COMMENT "--- Copying source resources"
    )
    add_dependencies(${PROJECT} copy)

endmacro()
