SRE_BUILD_SRC(sre_drivers "dummy.c")

function(SRE_DRIVERBUILD TARGET)
    project(${TARGET})
    add_library(${TARGET} OBJECT ${ARGN})

    target_include_directories(${TARGET} PRIVATE "../")
    target_include_directories(${TARGET} PRIVATE "${SRE_DIR}/common/imgui")
    target_link_libraries(${TARGET} PRIVATE sre)
    target_link_libraries(${TARGET} PRIVATE sre_private)
    target_link_libraries(sre_drivers PRIVATE ${TARGET} $<TARGET_OBJECTS:${TARGET}>) # Be careful with includes!
endfunction()

add_subdirectory(software)
add_subdirectory(sdl_renderer)

add_subdirectory(glcommon)
add_subdirectory(gl11)
add_subdirectory(gl21)
add_subdirectory(gl32)

if (WIN32)
    add_subdirectory("direct3d11")
    add_subdirectory("direct3d9")
    add_subdirectory("direct3d12 (joke)")
endif()