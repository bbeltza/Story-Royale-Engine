#ifndef SRE_HINTS_H
#define SRE_HINTS_H
#include <C_API.h>

/* List of all hints available */
/* 
    - SRE_HINT_RENDERDRIVER: `int`/`sre_renderDriver` ; May work being a char on little endian systems
        It choses what render driver to use, in the `sre_renderDriver` enum included in <Core/Render.h>
    - SRE_HINT_EXTERN_RENDERDRIVER: Pointer to a `sre_RenderDriverData`/`sre::RenderDriverHelper` ; It will be treated as read only, so it can be `const`
        It allows you to run a custom external render driver in your game, by pointing this hint into a valid `sre_RenderDriverData` structure.

    - SRE_HINT_ARG_HANDLER: `sreArgumentHandler` ; Pointer to a valid callack function that will run for every command line argument
                                in the engine's `main` function BEFORE initializing anything, with the ability to optionally use the original `argc` and `argv` values
                                It's used to handle arguments for your game, the function also returns a status code indicating whether the program should keep running or not.
                                If the function returns 0, then the program should continue, otherwise, exit.
                                Utilities for parsing arguments like this may be built in the future.
                            Optionally, if SRE_HINT_ARG_HANDLERS is present too, being an `int`, then it represents the amount of argument handlers in SRE_HINT_ARG_HANDLER,
                                assuming it is an array of argument handler functions. If it is not present, then only 1 argument handler is considered to be run.
    - SRE_HINT_IMGUI_GLUE: Used by sre_imguiglue so if you link it, you don't have to bother with it, nor you should
        It is a sre::ImGuiInterface inherited object (So it can be of any class type that INHERITS it).
        The interface definition is included in <ImGui.hpp> for C++ only since ImGui is included with C++
        This hint allows the integration with ImGui. In the class that you define, you must initialize ImGui in the `initialize` virutal method
        The `initialize` method returns the same set of codes as render driver initialization functions do. So include <Core/Render.h> for that.
*/
// This is it for now, I'm looking forward to expand this more


#if _WIN32
    #define SRE_EXPORT_HINT SRE_EXTERN_C_VAR __declspec(dllexport)
#elif __GNUC__ && 0 // It's probably.. not needed yet :)
    #define SRE_EXPORT_HINT __attribute__((__visibility__("default")))
#else
    #define SRE_EXPORT_HINT
#endif

typedef int (*sreArgumentHandler)(const char* arg, int argc, char* argv[]);

// Get the address of a hint declared by the user, `name` is a null-terminated string of the hint name and shouldn't include the `SRE_HINT_` prefix
// @return This method returns the address of the hint declared by the user with `SRE_EXPORT_HINT`, or `NULL` if the hint hasn't been declared by the user
SRE_EXTERN_C const void* sre_gethint(const char* name);

#ifdef __cplusplus
    namespace sre
    {
        inline const void* gethint(const char* name) { return sre_gethint(name); }
    }
#endif

#endif