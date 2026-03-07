set(SDL2TTF_VENDORED ON)
set(SDL2_VENDORED ON)
set(SDL2IMAGE_SAMPLES OFF)
set(SDL2IMAGE_INSTALL OFF)
set(FMT-TEST OFF)
set(FMT_UNICODE OFF)

# SDL libs
add_subdirectory(SDL)
add_subdirectory(SDL_ttf)
add_subdirectory(SDL_image)

# Rest of the lib(s)
add_subdirectory(stb)
#add_subdirectory(zlib)

include(imgui.cmake)

if(NOT BUILD_SHARED_LIBS)
    set(SDL_LIB_SUFFIX "-static")
    set(ZLIB_SUFFIX "static")
endif()

set(3RDPARTYLIBS
SDL2::SDL2${SDL_LIB_SUFFIX}
SDL2_ttf::SDL2_ttf${SDL_LIB_SUFFIX}
SDL2_image::SDL2_image${SDL_LIB_SUFFIX}

stb
imgui
#zlib${ZLIB_SUFFIX}
)

set(3RDPARTYLIBS ${3RDPARTYLIBS} PARENT_SCOPE)