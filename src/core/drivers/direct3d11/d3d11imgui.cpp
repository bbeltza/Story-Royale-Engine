#include "d3d11.hpp"
#include <ImGui.hpp>
#include <../imgui/backends/imgui_impl_dx11.cpp>

namespace sreD3D11
{
    struct ImGuiData: sre::ImGuiRenderInterface<Instance>
    {
        int Init(driver_type* inst, SDL_Window* window) override;
        void NewFrame(driver_type* inst) override;
        void RenderDrawData(driver_type* inst, ImDrawData* imDrawData) override;
        void Shutdown() override;
    };
}

SRE_EXTERN_C_VAR sreD3D11::ImGuiData sred3d11imgui{};

int sreD3D11::ImGuiData::Init(driver_type* inst, SDL_Window* window)
{
    if (!ImGui_ImplDX11_Init(inst->m_dxdevice, inst->m_dxdevicecontext))
        return SRE_RENDERSTATUS_FAILED;
    return SRE_RENDERSTATUS_SUCCEEDED;
}

void sreD3D11::ImGuiData::NewFrame(driver_type*) { ImGui_ImplDX11_NewFrame(); }
void sreD3D11::ImGuiData::RenderDrawData(driver_type*, ImDrawData* imDrawData) { ImGui_ImplDX11_RenderDrawData(imDrawData); }
void sreD3D11::ImGuiData::Shutdown() { ImGui_ImplDX11_Shutdown(); }