#ifndef SRE_DRIVER_OPENGL_H
#define SRE_DRIVER_OPENGL_H
#include <drivers.h>
#include <C_API.h>

#include <SDL_opengl.h>

SRE_CAPI_BEGIN

typedef struct sre_videoOpenGL
{
    SDL_GLContext context;
} sre_videoOpenGL;

typedef struct sre_GLtexture
{
    void* happy;
} sre_GLtexture;

extern bool sreopengl_init(sre_videodriver* video, SDL_Window* window);

#ifndef IMGUI_DISABLE
    extern const struct sre_videodriverImGuiInterface sreopengl_imgui;
#endif

SRE_CAPI_END

#endif