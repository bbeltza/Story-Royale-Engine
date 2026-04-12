#ifndef SRE_GLCOMMON_H
#define SRE_GLCOMMON_H

#include <SDL_opengl.h>
#include "glfuncs.h"

#include <Base/Log.h>

#define SRE_GLCALL(x) x; do { GLenum _err = SRE_GLGETERROR(); if (_err == GL_NO_ERROR) break; SRE_GLLOG("[OpenGL error]: '" #x "' failed: '%s' | line: %d file: '%s'", SRE_GLERRFMT(_err), __LINE__, __FILE__); } while (1)
#define SRE_GLGETERROR inst->glfuncs.GetError // Macro to the variable to hold glGetError, it's usually inst->glfuncs.GetError but it can be changed
#define SRE_GLLOG(...) sre_log(SRE_LOGCATEGORY_ERROR, __VA_ARGS__) // You'd need #include <Base/Log.h> for the default definition

#define SRE_GLCTXMAKE(ret) if (SDL_GL_MakeCurrent(inst->window, inst->context)) return ret
#define SRE_GLCTXCHECK assert(SDL_GL_GetCurrentContext() == inst->context) // Need #include <assert.h>

struct sregl_functions
{
    SRE_GLFUNCS
};

#ifdef __cplusplus
    extern "C" {
#endif

extern const char* SRE_GLERRFMT(GLenum err);
extern GLboolean sregl_loadfunctions(struct sregl_functions* funcs, void* (*pGetProcAddress)(const char* proc));

#ifdef __cplusplus
    }
#endif

#endif