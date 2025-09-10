set(SDL2TTF_VENDORED ON)
set(SDL2MIXER_VENDORED ON)
set(SDL2MIXER_VORBIS OFF)
set(SDL2_VENDORED ON)
set(SDL2IMAGE_SAMPLES OFF)
set(SDL2IMAGE_INSTALL OFF)
set(FMT-TEST OFF)

# SDL libs
add_subdirectory(SDL)
add_subdirectory(SDL_ttf)
add_subdirectory(SDL_image)
add_subdirectory(SDL_mixer)

# Rest of the libs
add_subdirectory(fmt)
add_subdirectory(stb)