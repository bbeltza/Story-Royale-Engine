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
    - SRE_HINT_ECS_ENTRYPOINT & SRE_HINT_GUI_ENTRYPOINT: These are functions (which will be interpreted with `void f(void)` as the signature) that run before setting up the 
        new object layer system for the ECS and GUI respectively. It's used by the ECS and GUI modules to initialize all of the necessary data to function, as a way to no
        longer rely on the internal engine anymore, so you shouldn't use them. But if you don't rely on those modules, then using them as a way of setting different entry points is possible.
    - SRE_HINT_SDL_WINDOWFLAGS: int, specifies which flags to use when creating the main window in SDL_CreateWindow. Just be careful with using them!
                                  Note that the SDL_WINDOW_HIDDEN flag will be automatically added during creation, even if it's not specified, unless 
                                  the SDL_WINDOW_SHOWN flag is specified. The main window is created by default with the SDL_WINDOW_OPENGL and SDL_WINDOW_HIDDEN flags.
                                These flags can be used to either have custom vulkan or metal support (with SDL_WINDOW_VULKAN/METAL), or to remove the SDL_WINDOW_OPENGL flag,
                                  or to add some minimal configuration to it.
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