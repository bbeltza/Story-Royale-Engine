#ifndef SRE_GLCOMMON_H
#define SRE_GLCOMMON_H

#include <SDL_opengl.h>
#include "glfuncs.h"

#include <Base/Log.h>

#define SRE_GLCALLR(x, ...) x; do { GLenum _err = SRE_GLGETERROR(); if (_err == GL_NO_ERROR) break; SRE_GLLOG("[OpenGL error]: '" #x "' failed: '%s' | line: %d file: '%s'", SRE_GLERRFMT(_err), __LINE__, __FILE__); __VA_ARGS__; } while (1)
#define SRE_GLCALLF(x) SRE_GLCALLR(x, return false)
#define SRE_GLCALL(x) SRE_GLCALLR(x)

#define SRE_GLGETERROR inst->glfuncs.GetError // Macro to the variable to hold glGetError, it's usually inst->glfuncs.GetError but it can be changed
#define SRE_GLLOG(...) sre_log(SRE_LOGCATEGORY_ERROR, __VA_ARGS__) // You'd need #include <Base/Log.h> for the default definition

#define SRE_GLCTXMAKE(ret) if (SDL_GL_MakeCurrent(inst->window, inst->context)) return ret
#define SRE_GLCTXCHECK assert(SDL_GL_GetCurrentContext() == inst->context) // Need #include <assert.h>

struct sregl_functions
{
    SRE_GLFUNCS
};

struct sregl_functions21
{
    SRE_GLFUNCS21
};

#ifdef __cplusplus
    extern "C" {
#endif

extern const char* SRE_GLERRFMT(GLenum err);
extern GLboolean sregl_loadfunctions(struct sregl_functions* funcs, void* (*pGetProcAddress)(const char* proc));
extern GLboolean sregl_loadfunctions21(struct sregl_functions21* funcs, void* (*pGetProcAddress)(const char* proc));

#ifdef __cplusplus
    }
#endif

#endif