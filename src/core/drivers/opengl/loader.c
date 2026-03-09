#include "opengl.h"

#define SRE_GL_LOADER SDL_GL_GetProcAddress
#undef SRE_GL_FUNC
#define SRE_GL_FUNC(t, x) inst->x = (t)SRE_GL_LOADER(#x); if (!inst->x) { sre_logsimple(SRE_LOGCATEGORY_ERROR, "[OPENGL]: Failed loading symbol: '" #x "'"); return false; } (void)0

extern bool SRE_GL_LOAD2(struct sre_GLfuncs2_1* inst)
{
    SRE_GL_FUNCTIONS_2_1

    return true;
}

extern bool SRE_GL_LOAD3(struct sre_GLfuncs3_2* inst)
{
    SRE_GL_FUNCTIONS_3_2

    return true;
}

const char* SRE_GL_FMTERR(GLenum err)
{
    #define SRE_GL_CASE(x) case x: return #x
    switch (err)
    {
        SRE_GL_CASE(GL_NO_ERROR);
        SRE_GL_CASE(GL_INVALID_ENUM);
        SRE_GL_CASE(GL_INVALID_VALUE);
        SRE_GL_CASE(GL_INVALID_OPERATION);
        SRE_GL_CASE(GL_STACK_OVERFLOW);
        SRE_GL_CASE(GL_STACK_UNDERFLOW);
        SRE_GL_CASE(GL_OUT_OF_MEMORY);
        SRE_GL_CASE(GL_INVALID_FRAMEBUFFER_OPERATION);
        default: 
        #if _WIN32
            __debugbreak();
        #endif
            return NULL;
    }
}