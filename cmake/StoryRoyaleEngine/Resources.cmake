macro(srEngine_nobind PROJECT)
	message("--- Linking ${PROJECT} with a no-binder")
	target_sources(${PROJECT} PRIVATE ${SRENGINE_DIR}/gen_src/empty_res.c)
	# target_link_libraries(${PROJECT} no_bind)
endmacro()

function(srEngine_link_resource PROJECT)
    get_target_property(INPUT ${PROJECT} RES_FOLDER)

    if(NOT INPUT)
	srEngine_nobind(${PROJECT})
        return()
    endif()


    if ("${ARGN}" MATCHES NO_BIND)
        get_target_property(OUTPUT ${PROJECT} RUNTIME_OUTPUT_DIRECTORY)

        file(MAKE_DIRECTORY ${OUTPUT}/__res)
        add_custom_target(
            copy_${PROJECT}
            COMMAND ${CMAKE_COMMAND} -E rm -rf ${OUTPUT}/__res
            COMMENT "--- Copying resources folder into ${OUTPUT}/__res..."
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${INPUT}
            ${OUTPUT}/__res
            )
	message("-- ${OUTPUT}")	
        message("-- ${CMAKE_BUILD_TYPE}")
        
        add_dependencies(${PROJECT} copy_${PROJECT})
	
	srEngine_nobind(${PROJECT})
    else()
        if (WIN32)
            set(IS_WIN32 1)
        else()
            set(IS_WIN32 0)
        endif()
        set(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}")
        add_custom_target(
        bind_${PROJECT}
        COMMENT "--- Binding source resources..."
        COMMAND ${PYTHON_COMMAND}resource_binder.py
        ${INPUT}
        ${OUTPUT}
        ${IS_WIN32}
        DEPENDS ${OUTPUT}/_res.c
        )

        file(WRITE ${OUTPUT}/_res.c)
        
        add_dependencies(${PROJECT} bind_${PROJECT})

        target_sources(${PROJECT} PRIVATE ${OUTPUT}/_res.c)
    endif()
endfunction()
