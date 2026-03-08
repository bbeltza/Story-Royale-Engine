#ifndef IMGUI_DISABLE

#include "opengl.h"

#include <backends/imgui_impl_sdl2.h>
#include <backends/imgui_impl_opengl2.cpp>

static bool sreopengl_imgui_init(const sre_videodriver* video)
{
    //sre_videoOpenGL* inst = static_cast<sre_videoOpenGL*>(video->userdata);

    if (!ImGui_ImplOpenGL2_Init())
        return false;
    
    return true;
}

static void sreopengl_imgui_newframe() { ImGui_ImplOpenGL2_NewFrame(); }
static void sreopengl_imgui_renderdrawdata(ImDrawData* data, const sre_videodriver*) { ImGui_ImplOpenGL2_RenderDrawData(data); }

extern "C" const sre_videodriverImGuiInterface sreopengl_imgui = {
    sreopengl_imgui_init,
    sreopengl_imgui_newframe,
    sreopengl_imgui_renderdrawdata
};

#endif