#include "glcommon.h"

extern const char* SRE_GLERRFMT(GLenum err)
{
    #define SRE_GLERRCASE(x) case x: return #x
    switch (err)
    {
        SRE_GLERRCASE(GL_NO_ERROR);
        SRE_GLERRCASE(GL_INVALID_ENUM);
        SRE_GLERRCASE(GL_INVALID_VALUE);
        SRE_GLERRCASE(GL_INVALID_OPERATION);
        SRE_GLERRCASE(GL_INVALID_FRAMEBUFFER_OPERATION);
        SRE_GLERRCASE(GL_OUT_OF_MEMORY);
        SRE_GLERRCASE(GL_STACK_UNDERFLOW);
        SRE_GLERRCASE(GL_STACK_OVERFLOW);
        default: return NULL;
    }
}

#undef SRE_GLPFNDEF
#define SRE_GLPFNDEF(t, x) funcs->x = (t)pGetProcAddress("gl" #x); if (!funcs->x) { SRE_GLLOG("Failed to load function: 'gl" #x"'"); return false; }
bool sregl_loadfunctions(struct sregl_functions* funcs, void* (*pGetProcAddress)(const char* proc))
{
    SRE_GLFUNCS

    return true;
}

bool sregl_loadfunctions21(struct sregl_functions21* funcs, void* (*pGetProcAddress)(const char* proc))
{
    SRE_GLFUNCS21

    return true;
}

bool sregl_loadfunctions32(struct sregl_functions32* funcs, void* (*pGetProcAddress)(const char* proc))
{
    SRE_GLFUNCS32

    return true;
}

bool sregl_loadfunctions11(struct sregl_functions11* funcs, void* (*pGetProcAddress)(const char* proc))
{
    SRE_GLFUNCS11

    return true;
}


extern bool sregl_commonsetup(sregl_cominst* inst, SDL_Window* window, struct sregl_functions* glfuncs)
{
    SDL_GLContext ctx = SDL_GL_CreateContext(window);
    if (!ctx)
        return false;
    
    if (!sregl_loadfunctions(glfuncs, SDL_GL_GetProcAddress))
    {
        SDL_GL_DeleteContext(inst->context);
        return false;
    }
    
    inst->context = ctx;
    inst->window = window;
    return true;
}

extern void sregl_commondestroy(sregl_cominst* inst)
{
    SDL_GL_DeleteContext(inst->context);
}