#include "d3d9.hpp"
#include <ImGui.hpp>
#include <../imgui/backends/imgui_impl_dx9.cpp>

static bool has_imgui = false;
namespace sreD3D9
{

    struct ImGuiData: sre::ImGuiRenderInterface<Instance>
    {
        int Init(driver_type* inst, SDL_Window* window) override;
        void NewFrame(driver_type* inst) override;
        void RenderDrawData(driver_type* inst, ImDrawData* imDrawData) override;
        void Shutdown() override;
    };
}

extern "C" sreD3D9::ImGuiData sred3d9imgui{};

int sreD3D9::ImGuiData::Init(driver_type* inst, SDL_Window* window)
{
    if (!ImGui_ImplDX9_Init(inst->m_dxdevice))
        return SRE_RENDERSTATUS_FAILED;
    
    has_imgui = true;
    return SRE_RENDERSTATUS_SUCCEEDED;
}

void sreD3D9::ImGuiData::NewFrame(driver_type*) { ImGui_ImplDX9_NewFrame(); }
void sreD3D9::ImGuiData::RenderDrawData(driver_type*, ImDrawData* imDrawData) { ImGui_ImplDX9_RenderDrawData(imDrawData); }
void sreD3D9::ImGuiData::Shutdown() { ImGui_ImplDX9_Shutdown(); }

void sreD3D9::Instance::_invalidateimgui()
{
    if (has_imgui) // You need to invalidate every object to reset the device for resizing. Do that if ImGui's there. The very fancy has_imgui variable tells that
        ImGui_ImplDX9_InvalidateDeviceObjects();
}