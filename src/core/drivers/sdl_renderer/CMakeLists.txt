project(sre_sdlrenderer)

add_library(sre_sdlrenderer "init.c" "drawing.c" "textures.c" "imgui.cpp")

target_link_libraries(sre_sdlrenderer PRIVATE sre_driverinterface)