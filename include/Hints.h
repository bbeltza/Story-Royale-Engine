#ifndef SRE_HINTS_H
#define SRE_HINTS_H
#include <C_API.h>

/* List of all hints available */
/* 
    - SRE_HINT_RENDERDRIVER: `int`/`sre_renderDriver` ; May work being a char on little endian systems
        It choses what render driver to use, in the `sre_renderDriver` enum included in <Core/Render.h>
*/
// This is it for now, I'm looking forward to expand this


#if _WIN32
    #define SRE_EXPORT_HINT SRE_EXTERN_C __declspec(dllexport)
#else
    #define SRE_EXPORT_HINT SRE_EXTERN_C
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