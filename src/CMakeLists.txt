macro (SRE_BUILD_SRC TARGET)
    project(${TARGET})

    add_library(${TARGET} STATIC ${ARGN})
    target_link_libraries(${TARGET} PRIVATE sre)
    target_link_libraries(${TARGET} PRIVATE sre_private)
endmacro()

add_subdirectory(core)
add_subdirectory(base)
add_subdirectory(main)
add_subdirectory(ecs)
add_subdirectory(gui)
add_subdirectory(presets)