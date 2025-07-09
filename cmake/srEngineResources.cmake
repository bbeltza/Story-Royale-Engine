macro(srEngine_set_resource_file PROJECT SRC)
    add_custom_target(
        copy ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/${SRC}/
        ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/${SRC}
        COMMENT "--- Copying source resources"
    )
    add_dependencies(${PROJECT} copy)

endmacro()
