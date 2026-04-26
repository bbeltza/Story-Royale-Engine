#include "gl32.h"
#include <ImGui.hpp>

namespace sregl32
{
    struct ImGuiData: sre::ImGuiRenderInterface<sregl21_inst>
    {
        int Init(driver_type* inst, SDL_Window* window) override;
        void NewFrame(driver_type* inst) override;
        void RenderDrawData(driver_type* inst, ImDrawData* imDrawData) override;
        void Shutdown() override;
    };
}

#if SRE_STATIC_GL || 1
#include <../imgui/backends/imgui_impl_opengl3.h> // Using header only, assuming gl21 gets compiled with the source code

int sregl32::ImGuiData::Init(driver_type* inst, SDL_Window* window)
{
    assert(inst->common21.common.window == window);
    if (SDL_GL_MakeCurrent(window, inst->common21.common.context) != 0)
        return SRE_RENDERSTATUS_FAILED;

    if (!ImGui_ImplOpenGL3_Init())
        return SRE_RENDERSTATUS_FAILED;
    return SRE_RENDERSTATUS_SUCCEEDED;
}

void sregl32::ImGuiData::NewFrame(driver_type* inst) { ImGui_ImplOpenGL3_NewFrame(); }
void sregl32::ImGuiData::RenderDrawData(driver_type* inst, ImDrawData* imDrawData) { ImGui_ImplOpenGL3_RenderDrawData(imDrawData); }
void sregl32::ImGuiData::Shutdown() { ImGui_ImplOpenGL3_Shutdown(); }

#else

int sregl32::ImGuiData::Init(driver_type*, SDL_Window*) { return SRE_RENDERSTATUS_UNSUPPORTED; }

void sregl32::ImGuiData::NewFrame(driver_type*) {}
void sregl32::ImGuiData::RenderDrawData(driver_type*, ImDrawData*) {}
void sregl32::ImGuiData::Shutdown() {}

#endif

extern "C" sregl32::ImGuiData sregl32imgui;
sregl32::ImGuiData sregl32imgui;