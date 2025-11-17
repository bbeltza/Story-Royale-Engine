macro(srEngine_nobind PROJECT)
	message("--- Linking ${PROJECT} with a no-binder")
	target_link_libraries(${PROJECT} PUBLIC no_bind)
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
        if (WIN32 OR UNIX)
            set(WANT_C 0)
        else()
            set(WANT_C 1)
        endif()
        set(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}")
        add_custom_target(
        bind_${PROJECT}
        COMMENT "--- Binding source resources..."
        COMMAND ${PYTHON_COMMAND}resource_binder.py
        ${INPUT}
        ${OUTPUT}
        ${WANT_C}
        DEPENDS ${OUTPUT}/_res.c
        DEPENDS ${OUTPUT}/_res.dat
        )

        file(APPEND ${OUTPUT}/_res.c)
        file(APPEND ${OUTPUT}/_res.dat)
        if (WIN32)
            file(WRITE ${OUTPUT}/_res.rc "GAME_RES	RCDATA	\"_res.dat\"")
        endif()
        
        add_dependencies(${PROJECT} bind_${PROJECT})

        if (WIN32)
            target_sources(${PROJECT} PUBLIC ${OUTPUT}/_res.rc)
            srEngine_nobind(${PROJECT})
        elseif(UNIX)
            configure_file(${SRENGINE_DIR}/asm/res.s.in ${OUTPUT}/_res.s)
            set_property(SOURCE ${OUTPUT}/_res.s PROPERTY LANGUAGE C)
            target_sources(${PROJECT} PUBLIC ${OUTPUT}/_res.s)
        else()
            target_sources(${PROJCET} PUBLIC ${OUTPUT}/_res.c)
        endif()
    endif()
endfunction()
