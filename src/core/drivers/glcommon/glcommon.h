#ifndef SRE_GLCOMMON_H
#define SRE_GLCOMMON_H

#include <SDL_opengl.h>
#include <SDL_video.h>

#include "glfuncs.h"

#include <Core/Render.h>
#include <Base/Log.h>

#include <stdlib.h>
#include <stdio.h>

#if !defined(NDEBUG)
    #define SRE_GLCALLR(x, ...) x; do { GLenum _err = SRE_GLGETERROR(); if (_err == GL_NO_ERROR) break; SRE_GLLOG("[OpenGL error]: '" #x "' failed: '%s' | line: %d file: '%s'", SRE_GLERRFMT(_err), __LINE__, __FILE__); __VA_ARGS__; } while (1)
#else
    #define SRE_GLCALLR(x, ...) x
#endif

#define SRE_GLCALLF(x) SRE_GLCALLR(x, return false)
#define SRE_GLCALLC(x) SRE_GLCALLR(x, return SRE_RENDERSTATUS_FAILED)
#define SRE_GLCALL(x) SRE_GLCALLR(x)

#define SRE_GLGETERROR inst->glfuncs.GetError // Macro to the variable to hold glGetError, it's usually inst->glfuncs.GetError but it can be changed
#define SRE_GLLOG(...) fprintf(stderr, __VA_ARGS__)


struct sregl_functions
{
    SRE_GLFUNCS
};

struct sregl_functions21
{
    SRE_GLFUNCS21
};

struct sregl_functions32
{
    SRE_GLFUNCS32
};

struct sregl_functions11
{
    SRE_GLFUNCS11
};

typedef struct sregl_cominst
{
    SDL_GLContext context;
    SDL_Window* window;
} sregl_cominst;

// Texture structure (it is or will certainly be shared across every OpenGL implementation)
typedef struct sregl_texture
{
    GLint gltex;
} sregl_texture;

#ifdef __cplusplus
    extern "C" {
#endif

extern const char* SRE_GLERRFMT(GLenum err);
extern bool sregl_loadfunctions(struct sregl_functions* funcs, void* (*pGetProcAddress)(const char* proc));
extern bool sregl_loadfunctions21(struct sregl_functions21* funcs, void* (*pGetProcAddress)(const char* proc));
extern bool sregl_loadfunctions32(struct sregl_functions32* funcs, void* (*pGetProcAddress)(const char* proc));
extern bool sregl_loadfunctions11(struct sregl_functions11* funcs, void* (*pGetProcAddress)(const char* proc));

extern int sregl_commonsetup(sregl_cominst* inst, SDL_Window* window, struct sregl_functions* glfuncs);
extern void sregl_commondestroy(sregl_cominst* inst);

extern bool sregl_texture_setup(struct sregl_functions* glfuncs, sregl_texture* texture, sre_pixelFormat format, int w, int h, sre_pixelFormat* outformat);
extern bool sregl_texture_update(struct sregl_functions* glfuncs, sregl_texture* texture, const sre_rect2Di* region, const void* pixels, int pitch);
extern void sregl_texture_destroy(struct sregl_functions* glfuncs, sregl_texture* texture);

extern void sregl_set_vsync(bool enable);
extern void sregl_set_clipstate(struct sregl_functions* glfuncs, SDL_Window* window, const sre_rect2Di* rectangle);
extern bool sregl_set_blendstate(struct sregl_functions* glfuncs, sre_blendMode blendmode);

#define SREGL_PRESENT(x) SDL_GL_SwapWindow(x.window);
#define SREGL_CLEAR(glfuncs, color) SRE_GLCALLF(glfuncs.ClearColor(color[0], color[1], color[2], 1)); \
                                    SRE_GLCALLF(glfuncs.Clear(GL_COLOR_BUFFER_BIT))

#ifdef __cplusplus
    }
#endif

#endif