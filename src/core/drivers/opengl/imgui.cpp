#ifndef IMGUI_DISABLE

#include "opengl.h"

#include <backends/imgui_impl_sdl2.h>
#include <backends/imgui_impl_opengl3.h>

static bool sreopengl_imgui_init(const sre_videodriver* video)
{
    //sre_videoOpenGL* inst = static_cast<sre_videoOpenGL*>(video->userdata);

    if (!ImGui_ImplOpenGL3_Init())
        return false;
    
    return true;
}

static void sreopengl_imgui_newframe() { ImGui_ImplOpenGL3_NewFrame(); }
static void sreopengl_imgui_renderdrawdata(ImDrawData* data, const sre_videodriver* video)
{
    const sre_videoOpenGL* inst = static_cast<sre_videoOpenGL*>(video->userdata);
    GLint last_program;
    GLint last_buffer;

    SRE_GL_CALL(glGetIntegerv(GL_CURRENT_PROGRAM, &last_program));
    SRE_GL_CALL(glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &last_buffer));
    SRE_GL_CALL(inst->funcs2.glUseProgram(0));
    SRE_GL_CALL(inst->funcs2.glBindBuffer(GL_ARRAY_BUFFER, 0));
    
    ImGui_ImplOpenGL3_RenderDrawData(data);
    SRE_GL_CALL(inst->funcs2.glUseProgram(last_program));
    SRE_GL_CALL(inst->funcs2.glBindBuffer(GL_ARRAY_BUFFER, last_buffer));
}

extern "C" const sre_videodriverImGuiInterface sreopengl_imgui = {
    sreopengl_imgui_init,
    sreopengl_imgui_newframe,
    sreopengl_imgui_renderdrawdata
};

#endif