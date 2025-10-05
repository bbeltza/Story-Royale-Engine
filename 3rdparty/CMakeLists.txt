set(SDL2TTF_VENDORED ON)
set(SDL2_VENDORED ON)
set(SDL2IMAGE_SAMPLES OFF)
set(SDL2IMAGE_INSTALL OFF)
set(FMT-TEST OFF)

# SDL libs
add_subdirectory(SDL)
add_subdirectory(SDL_ttf)
add_subdirectory(SDL_image)

# Rest of the libs
add_subdirectory(fmt)
add_subdirectory(stb)

# ImGui

project(ImGui)

file(GLOB SOURCES "imgui/*cpp" "imgui/*hpp")
add_library(ImGui
    ${SOURCES}
    "imgui/backends/imgui_impl_sdl2.cpp"
    "imgui/backends/imgui_impl_sdl2.h"
    "imgui/backends/imgui_impl_sdlrenderer2.cpp"
    "imgui/backends/imgui_impl_sdlrenderer2.h"
    )
target_include_directories(ImGui PUBLIC "imgui")
target_include_directories(ImGui PUBLIC "SDL/include")