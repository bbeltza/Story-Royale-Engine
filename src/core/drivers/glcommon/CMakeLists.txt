SRE_DRIVERBUILD(sre_glcommon "glcommon.c" "glcommonw.c") # It's technically not a driver but it is still useful

project(sre_opengl)
add_library(sre_opengl INTERFACE)

find_package(OpenGL)

if (OpenGL_FOUND)
    target_compile_definitions(sre_opengl INTERFACE SRE_STATIC_GL=1)
    target_link_libraries(sre_opengl INTERFACE OpenGL::GL)
endif()