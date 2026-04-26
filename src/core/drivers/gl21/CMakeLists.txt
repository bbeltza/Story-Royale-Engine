SRE_DRIVERBUILD(sre_gl21 "gl21main.c" "gl21draw.c" "gl21state.c" "gl21texture.c" "gl21buffers.c" "gl21imgui.cpp")
target_link_libraries(sre_gl21 PRIVATE sre_opengl)