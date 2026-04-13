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
#define SRE_GLPFNDEF(t, x) funcs->x = (t)pGetProcAddress("gl" #x); if (!funcs->x) { SRE_GLLOG("Failed to load function: 'gl" #x"'"); return GL_FALSE; }
extern GLboolean sregl_loadfunctions(struct sregl_functions* funcs, void* (*pGetProcAddress)(const char* proc))
{
    SRE_GLFUNCS

    return GL_TRUE;
}

extern GLboolean sregl_loadfunctions21(struct sregl_functions21* funcs, void* (*pGetProcAddress)(const char* proc))
{
    SRE_GLFUNCS21

    return GL_TRUE;
}