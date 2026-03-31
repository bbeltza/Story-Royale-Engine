set(SDL2_VENDORED ON)
set(SDL2TTF_VENDORED ON)
set(SDL2IMAGE_SAMPLES OFF)

set(SDL2TTF_INSTALL ON)
set(SDL2IMAGE_INSTALL ON)
set(SDL2_DISABLE_INSTALL OFF CACHE BOOL "Disable SDL2 install targets" FORCE)

# SDL libs
add_subdirectory(SDL)

add_subdirectory(SDL_ttf)
add_subdirectory(SDL_image)

# Rest of the lib(s)
add_subdirectory(stb)
#add_subdirectory(zlib)

include(imgui.cmake)