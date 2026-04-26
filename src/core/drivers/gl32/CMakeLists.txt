SRE_DRIVERBUILD(sre_gl32 "gl32main.c" "gl32draw.c" "gl32state.c" "gl32texture.c" "gl32buffers.c" "gl32imgui.cpp")
target_link_libraries(sre_gl32 PRIVATE sre_opengl)
