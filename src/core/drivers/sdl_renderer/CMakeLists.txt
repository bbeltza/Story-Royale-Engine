project(sre_sdlrenderer)

add_library(sre_sdlrenderer "init.c" "drawing.c")

target_link_libraries(sre_sdlrenderer PRIVATE sre_driverinterface)