find_package(OpenGL)
if (NOT OpenGL_FOUND)
    message("-- OpenGL HASN'T been found. Please, install the required packages if you require OpenGL")
    return()
endif()

SRE_DRIVERBUILD(sre_opengl "init.c" "imgui.cpp")
target_link_libraries(sre_opengl PRIVATE OpenGL::GL)