#ifndef IMGUI_DISABLE

#include "sdl_renderer.h"

#include <backends/imgui_impl_sdl2.h>
#include <backends/imgui_impl_sdlrenderer2.cpp>

static bool sresdlrenderer_imgui_init(const sre_videodriver* video)
{
    SDL_Renderer* renderer = static_cast<SDL_Renderer*>(video->userdata);
    SDL_Window* window = SDL_RenderGetWindow(renderer);
    if (!window)
        return false;

    if (!ImGui_ImplSDLRenderer2_Init(renderer))
        return false;
    
    return true;
}

static void sresdlrenderer_imgui_newframe() { ImGui_ImplSDLRenderer2_NewFrame(); }
static void sresdlrenderer_imgui_renderdrawdata(ImDrawData* data, const sre_videodriver* video) { ImGui_ImplSDLRenderer2_RenderDrawData(data, static_cast<SDL_Renderer*>(video->userdata)); }

extern "C" const sre_videodriverImGuiInterface sresdlrenderer_imgui = {
    sresdlrenderer_imgui_init,
    sresdlrenderer_imgui_newframe,
    sresdlrenderer_imgui_renderdrawdata
};

#endif