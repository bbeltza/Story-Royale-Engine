set(SDL2_VENDORED ON)
set(SDL2IMAGE_SAMPLES OFF)

if (SRE_INSTALL)
    set(SRE_NOINSTALL OFF)
else()
    set(SRE_NOINSTALL ON)
endif()

set(SDL2IMAGE_INSTALL ${SRE_INSTALL})
set(SDL2_DISABLE_INSTALL ${SRE_NOINSTALL} CACHE BOOL "Disable SDL2 install targets" FORCE)

# SDL libs
add_subdirectory(SDL)

add_subdirectory(SDL_image)

# Rest of the lib(s)
add_subdirectory(freetype)
#add_subdirectory(zlib)