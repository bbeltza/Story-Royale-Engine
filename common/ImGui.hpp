#ifndef SRE_IMGUI_HPP
#define SRE_IMGUI_HPP

#ifndef IMGUI_API // Avoid double including common/'s ImGui header if 3rdparty's one is already included
    #include <imgui/imgui.h>
#endif

#include <SDL_video.h>
#include <SDL_events.h>

namespace sre
{
    // Container containing data to initialize ImGui, if wanted. To be used with the SRE_HINT_IMGUIGLUE hint
    struct ImGuiInterface
    {
        virtual int initialize(SDL_Window* window, void* renderdriver, int renderindex) = 0;
        virtual void shutdown() = 0;

        virtual bool on_event(SDL_Event* ev) = 0;

        virtual void begin_frame(void) = 0;
        virtual void end_frame(void) = 0;
    };

    template <typename T = void>
    struct ImGuiRenderInterface
    {
        using driver_type = T;

        virtual int Init(driver_type* inst, SDL_Window* window) = 0;
        virtual void NewFrame(driver_type* inst) = 0;
        virtual void RenderDrawData(driver_type* inst, ImDrawData* imDrawData) = 0;
        virtual void Shutdown() = 0;
    };
}


#endif