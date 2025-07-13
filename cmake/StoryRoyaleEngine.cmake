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

function(srEngine_link_target target)
    if (NOT TARGET ${target}_res)
            message("NOT linking with resources bound")
            target_link_libraries(${target} no_bind)
    endif()
    target_link_libraries(${target} StoryRoyaleEngine)
endfunction()