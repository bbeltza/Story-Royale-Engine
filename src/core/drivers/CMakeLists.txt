include(sre/drivers)

add_subdirectory(sdl_renderer)
#add_subdirectory(opengl)

if (WIN32)
    #add_subdirectory("direct3d12 (joke)")
    add_subdirectory("direct3d11")
endif()