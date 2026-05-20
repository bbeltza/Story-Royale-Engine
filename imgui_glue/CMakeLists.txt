project(sre_imguigluelib)
project(sre_imgui)

add_library(sre_imgui STATIC "imgui_srcs.cpp")
target_include_directories(sre_imgui PRIVATE ${SRE_DIR}/3rdparty/imgui)
target_include_directories(sre_imgui PRIVATE ${SRE_DIR}/3rdparty/SDL/include)

add_library(sre_imguigluelib STATIC "imgui_glue.cpp")
target_link_libraries(sre_imguigluelib PRIVATE sre)
target_link_libraries(sre_imguigluelib PRIVATE sre_private)
target_include_directories(sre_imguigluelib PRIVATE ${SRE_DIR}/3rdparty/imgui)

install_wrapper(TARGETS sre_imguigluelib)