#include "sdlrenderer.h"
#include <ImGui.hpp>

namespace sresdlrenderer
{
    struct ImGuiData: sre::ImGuiRenderInterface<sresdlrenderer_inst>
    {
        int Init(driver_type* inst, SDL_Window* window) override;
        void NewFrame(driver_type* inst) override;
        void RenderDrawData(driver_type* inst, ImDrawData* imDrawData) override;
        void Shutdown() override;
    };
}

#include <../imgui/backends/imgui_impl_sdlrenderer2.cpp>

int sresdlrenderer::ImGuiData::Init(driver_type* inst, SDL_Window* window)
{
    if (!ImGui_ImplSDLRenderer2_Init(inst->renderer))
        return SRE_RENDERSTATUS_FAILED;
    return SRE_RENDERSTATUS_SUCCEEDED;
}

void sresdlrenderer::ImGuiData::NewFrame(driver_type* inst) { ImGui_ImplSDLRenderer2_NewFrame(); }
void sresdlrenderer::ImGuiData::RenderDrawData(driver_type* inst, ImDrawData* imDrawData) { ImGui_ImplSDLRenderer2_RenderDrawData(imDrawData, inst->renderer); }
void sresdlrenderer::ImGuiData::Shutdown() { ImGui_ImplSDLRenderer2_Shutdown(); }

extern "C" sresdlrenderer::ImGuiData sresdlrendererimgui;
sresdlrenderer::ImGuiData sresdlrendererimgui;