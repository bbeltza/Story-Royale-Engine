#include <Hints.h>

#include <backends/imgui_impl_sdl2.h>
#include <ImGui.hpp>

#include <Core/Render.h>

#include <SDL_events.h>

struct sreImGui_glue: sre::ImGuiInterface
{
    int initialize(SDL_Window* window, void* renderdriver, int renderindex) override;
    void shutdown() override;
    //
    bool on_event(SDL_Event* ev) override;
    //
    void begin_frame() override;
    void end_frame() override;

    private:
        sre::ImGuiRenderInterface<>* render;
        void* renderinst;
};

SRE_EXPORT_HINT sreImGui_glue SRE_HINT_IMGUI_GLUE = {};

#define IMGUI_RENDERDRIVER(x) sre##x##imgui
#define IMGUI_RENDERDRIVERDECL(x) extern "C" void* IMGUI_RENDERDRIVER(x)
#define IMGUI_RENDERDRIVERPTR(x) reinterpret_cast<sre::ImGuiRenderInterface<>*>(&IMGUI_RENDERDRIVER(x))

IMGUI_RENDERDRIVERDECL(sdlrenderer);
IMGUI_RENDERDRIVERDECL(gl11);
IMGUI_RENDERDRIVERDECL(gl21);
IMGUI_RENDERDRIVERDECL(gl32);
IMGUI_RENDERDRIVERDECL(d3d9);
IMGUI_RENDERDRIVERDECL(d3d11);
IMGUI_RENDERDRIVERDECL(d3d12);

static sre::ImGuiRenderInterface<>* render_drivers[] = {
    #if _WIN32
        IMGUI_RENDERDRIVERPTR(d3d11),
        IMGUI_RENDERDRIVERPTR(d3d9),
    #endif
    IMGUI_RENDERDRIVERPTR(gl32),
    IMGUI_RENDERDRIVERPTR(gl21),
    IMGUI_RENDERDRIVERPTR(gl11),
    IMGUI_RENDERDRIVERPTR(sdlrenderer),
    NULL,
    #ifdef SRE_USEDRIVER_D3D12
        IMGUI_RENDERDRIVERPTR(d3d12)
    #endif
};


int sreImGui_glue::initialize(SDL_Window* window, void* renderdriver, int renderindex)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    sre::ImGuiRenderInterface<>* driver;
    if (renderindex < 0)
    {
        // Find for external render ImGui driver hint
        auto imguidriverhint = static_cast<sre::ImGuiRenderInterface<>* const *>(sre::gethint("IMGUI_EXTERN_RENDERDRIVER"));
        if (!imguidriverhint)
            return SRE_RENDERSTATUS_UNSUPPORTED;

        driver = *imguidriverhint;
    }
    else
        driver = render_drivers[renderindex];
    
    if (!driver)
        return SRE_RENDERSTATUS_UNSUPPORTED;

    if (!ImGui_ImplSDL2_InitForOther(window))
        return SRE_RENDERSTATUS_FAILED;
    
    int renderstatus = driver->Init(renderdriver, window);
    if (renderstatus != SRE_RENDERSTATUS_SUCCEEDED)
    {
        ImGui_ImplSDL2_Shutdown();
        return renderstatus;
    }

    this->render = driver;
    this->renderinst = renderdriver;
    return SRE_RENDERSTATUS_SUCCEEDED;
}

void sreImGui_glue::shutdown()
{
    render->Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
}

void sreImGui_glue::begin_frame()
{
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();
    // You can uncomment this to show a little demo window but note that every ImGui function will be safe to call after this begin_frame call
    // You can call any ImGui function from here on inside render functions
    ImGui::ShowDemoWindow();
}

#include <Base/Log.h>

void sreImGui_glue::end_frame()
{
    render->NewFrame(renderinst);
    
    ImGui::Render();

    /* Exploring ImGui::GetDrawData()...
    auto p_drawdata = ImGui::GetDrawData();
    for (auto& cmdlist : p_drawdata->CmdLists)
    {
        for (auto& cmdbuf : cmdlist->CmdBuffer)
            sre::log("%zu", cmdbuf.IdxOffset);
    }
    */

    render->RenderDrawData(renderinst, ImGui::GetDrawData());

}

//

bool sreImGui_glue::on_event(SDL_Event* ev)
{
    ImGui_ImplSDL2_ProcessEvent(ev);
    ImGuiIO& io = ImGui::GetIO();
    if (ev->type == SDL_MOUSEBUTTONDOWN && io.WantCaptureMouse)
        return false;
    if (ev->type == SDL_KEYDOWN && io.WantTextInput)
        return false;

    return true;
}