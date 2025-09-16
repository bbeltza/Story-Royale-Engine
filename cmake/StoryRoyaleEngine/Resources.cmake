function(srEngine_link_resource PROJECT)
    get_target_property(INPUT ${PROJECT} RES_FOLDER)

    if(NOT INPUT)
        target_link_libraries(${PROJECT} no_bind)
        return()
    endif()


    if ("${ARGN}" MATCHES NO_BIND)
        get_target_property(OUTPUT ${PROJECT} RUNTIME_OUTPUT_DIRECTORY)

        file(MAKE_DIRECTORY ${OUTPUT}/_res)
        add_custom_target(
            copy_${PROJECT}
            COMMAND ${CMAKE_COMMAND} -E rm -rf ${OUTPUT}/_res
            COMMENT "--- Copying resources folder into ${OUTPUT}/_res..."
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${INPUT}
            ${OUTPUT}/_res
            )
        message("-- ${OUTPUT}")
        message("-- ${CMAKE_BUILD_TYPE}")
        
        add_dependencies(${PROJECT} copy_${PROJECT})

        message("--- Linking ${target} with a no-binder")
        target_link_libraries(${PROJECT} no_bind)
    else()
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
    endif()
endfunction()