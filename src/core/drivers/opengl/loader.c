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