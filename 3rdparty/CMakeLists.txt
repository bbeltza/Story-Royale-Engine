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

# Rest of the libs
add_subdirectory(stb)
add_subdirectory(zlib)

if (HAVE_STDIO_H)
    target_compile_definitions(StoryRoyaleEngine PUBLIC "SDL_HAVE_STDIO_H")
endif()