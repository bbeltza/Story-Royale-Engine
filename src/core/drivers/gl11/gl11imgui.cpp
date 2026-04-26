#include "gl11.h"
#include <ImGui.hpp>


namespace sregl11
{
    struct ImGuiData: sre::ImGuiRenderInterface<sregl11_inst>
    {
        int Init(driver_type* inst, SDL_Window* window) override;
        void NewFrame(driver_type* inst) override;
        void RenderDrawData(driver_type* inst, ImDrawData* imDrawData) override;
        void Shutdown() override;
    };
}

#if SRE_STATIC_GL
#include <../imgui/backends/imgui_impl_opengl2.cpp>

int sregl11::ImGuiData::Init(driver_type* inst, SDL_Window* window)
{
    assert(inst->common.window == window);
    if (SDL_GL_MakeCurrent(window, inst->common.context) != 0)
        return SRE_RENDERSTATUS_FAILED;

    // OpenGL 1.1's ImGui will be implemented with gl2 so if initialization fails we return RENDERSTATUS_UNSUPPORTED
    if (!ImGui_ImplOpenGL2_Init())
        return SRE_RENDERSTATUS_UNSUPPORTED;
    return SRE_RENDERSTATUS_SUCCEEDED;
}

void sregl11::ImGuiData::NewFrame(driver_type* inst) { ImGui_ImplOpenGL2_NewFrame(); }
void sregl11::ImGuiData::RenderDrawData(driver_type* inst, ImDrawData* imDrawData) { ImGui_ImplOpenGL2_RenderDrawData(imDrawData); }
void sregl11::ImGuiData::Shutdown() { ImGui_ImplOpenGL2_Shutdown(); }

#else

int sregl11::ImGuiData::Init(driver_type*, SDL_Window*) { return SRE_RENDERSTATUS_UNSUPPORTED; }

void sregl11::ImGuiData::NewFrame(driver_type*) {}
void sregl11::ImGuiData::RenderDrawData(driver_type*, ImDrawData*) {}
void sregl11::ImGuiData::Shutdown() {}

#endif

extern "C" sregl11::ImGuiData sregl11imgui;
sregl11::ImGuiData sregl11imgui;