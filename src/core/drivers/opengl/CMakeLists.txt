if (ANDROID)
    set(SRE_OPENGL_LIB GLESv3)
else()
    find_package(OpenGL REQUIRED)
    if (NOT OpenGL_FOUND)
        message("-- OpenGL HASN'T been found. Please, install the required packages if you require OpenGL")
        return()
    endif()

    set(SRE_OPENGL_LIB OpenGL::GL)
endif()

SRE_DRIVERBUILD(sre_opengl "init.c" "draw.c" "textures.c" "setup.c" "loader.c" "imgui.cpp" "imgui_impl3.cpp")
target_link_libraries(sre_opengl PRIVATE ${SRE_OPENGL_LIB})