// ImGui is not ever defined as a cmake target or project, but is instead included here as for all of the necessary sources (excluding the render driver backends in which THEY will have to do it)
#include <imgui.cpp>
#include <imgui_draw.cpp>
#include <imgui_tables.cpp>
#include <imgui_widgets.cpp>

#if _WIN32 // Capturing the mouse outside the window can only be called on the main thread, at least on windows. We're calling ImGui::NewFrame outside of the main thread so we cannot use that
    #include <SDL_mouse.h>
    #define SDL_CaptureMouse(b) (void)0
#endif
#include <backends/imgui_impl_sdl2.cpp>

// Include this if you want ImGui::ShowDemoWindow along with many other examples
#include <imgui_demo.cpp>