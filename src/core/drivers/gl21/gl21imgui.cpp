#include "gl21.h"
#include <ImGui.hpp>

// Using OpenGL 3's ImGui backend since OpenGL 2 is for more legacy code so it breaks
// So if initialization fails we return RENDERSTATUS_UNSUPPORTED

namespace sregl21
{
    struct ImGuiData: sre::ImGuiRenderInterface<sregl21_inst>
    {
        int Init(driver_type* inst, SDL_Window* window) override;
        void NewFrame(driver_type* inst) override;
        void RenderDrawData(driver_type* inst, ImDrawData* imDrawData) override;
        void Shutdown() override;
    };
}

#if SRE_STATIC_GL
#include <../imgui/backends/imgui_impl_opengl3.cpp>

int sregl21::ImGuiData::Init(driver_type* inst, SDL_Window* window)
{
    assert(inst->common21.common.window == window);
    if (SDL_GL_MakeCurrent(window, inst->common21.common.context) != 0)
        return SRE_RENDERSTATUS_FAILED;

    if (!ImGui_ImplOpenGL3_Init())
        return SRE_RENDERSTATUS_UNSUPPORTED;
    return SRE_RENDERSTATUS_SUCCEEDED;
}

void sregl21::ImGuiData::NewFrame(driver_type* inst) { ImGui_ImplOpenGL3_NewFrame(); }
void sregl21::ImGuiData::Shutdown() { ImGui_ImplOpenGL3_Shutdown(); }
void sregl21::ImGuiData::RenderDrawData(driver_type* inst, ImDrawData* imDrawData) { ImGui_ImplOpenGL3_RenderDrawData(imDrawData); }

#else

int sregl21::ImGuiData::Init(driver_type*, SDL_Window*) { return SRE_RENDERSTATUS_UNSUPPORTED; }

void sregl21::ImGuiData::NewFrame(driver_type*) {}
void sregl21::ImGuiData::RenderDrawData(driver_type*, ImDrawData*) {}
void sregl21::ImGuiData::Shutdown() {}

#endif

extern "C" sregl21::ImGuiData sregl21imgui;
sregl21::ImGuiData sregl21imgui;