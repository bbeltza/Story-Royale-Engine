#ifndef SRE_HINTS_H
#define SRE_HINTS_H
#include <C_API.h>

/* List of all hints available */
/* 
    - SRE_HINT_RENDERDRIVER: `int`/`sre_renderDriver` ; May work being a char on little endian systems
        It choses what render driver to use, in the `sre_renderDriver` enum included in <Core/Render.h>
    - SRE_HINT_IMGUI_GLUE: Used by sre_imguiglue so if you link it, you don't have to bother with it, nor you should
        It is a sre::ImGuiInterface inherited object (So it can be of any class type that INHERITS it).
        The interface definition is included in <ImGui.hpp> for C++ only since ImGui is included with C++
        This hint allows the integration with ImGui. In the class that you define, you must initialize ImGui in the `initialize` virutal method
        The `initialize` method returns the same set of codes as render driver initialization functions do. So include <Core/Render.h> for that.
*/
// This is it for now, I'm looking forward to expand this more


#if _WIN32
    #define SRE_EXPORT_HINT SRE_EXTERN_C __declspec(dllexport)
#elif __GNUC__ && 0 // It's probably.. not needed yet :)
    #define SRE_EXPORT_HINT __attribute__((__visibility__("default")))
#else
    #define SRE_EXPORT_HINT
#endif

// Get the address of a hint declared by the user, `name` is a null-terminated string and shouldn't include the `SRE_HINT_` prefix
// @return This method returns the address of the hint declared by the user with `SRE_EXPORT_HINT`, or `NULL` if the hint hasn't been declared by the user
SRE_EXTERN_C const void* sre_gethint(const char* name);

#ifdef __cplusplus
    namespace sre
    {
        inline const void* gethint(const char* name) { return sre_gethint(name); }
    }
#endif

#endif