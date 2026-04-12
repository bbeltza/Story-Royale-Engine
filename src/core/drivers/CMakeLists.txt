include(sre/drivers)

add_subdirectory(sdl_renderer)

add_subdirectory(glcommon)
add_subdirectory(gl21)
#add_subdirectory(opengl)

if (WIN32)
    #add_subdirectory("direct3d12 (joke)")
    add_subdirectory("direct3d11")
endif()