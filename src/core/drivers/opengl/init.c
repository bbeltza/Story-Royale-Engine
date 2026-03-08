#include "opengl.h"

#include <standard>
#include <utils/mem.h>

extern const struct sre_videodriverInterface sreopengl_interface;

// Okay so we need to recreate the SDL window, it's an external function that SDL's internals provide to create OpenGL contexts for the renderer
//      since creating OpenGL contexts require the SDL_WINDOW_OPENGL flag but the Window may not have it
// Just, sad that it is not exported... So on Windows if we want to dynamically link SDL, we cannot use it :(
extern int SDL_RecreateWindow(SDL_Window *window, Uint32 flags);

extern bool sreopengl_init(sre_videodriver* video, SDL_Window* window)
{
    assert(video != NULL);

    Uint32 win_flags = SDL_GetWindowFlags(window);
    if (!(win_flags & SDL_WINDOW_OPENGL))
    {
        win_flags |= SDL_WINDOW_OPENGL;
        if (SDL_RecreateWindow(window, win_flags) < 0)
            return false;
    }

    sre_videoOpenGL* inst = sre_new(sizeof(sre_videoOpenGL));
    inst->context = SDL_GL_CreateContext(window);
    if (!inst->context)
        goto FAIL;

    video->userdata = inst;
    video->interface = &sreopengl_interface;
    video->texture_size = sizeof(sre_GLtexture);
    #ifndef IMGUI_DISABLE
        video->imgui = &sreopengl_imgui;
    #endif

    return true;
    FAIL:
        sre_delete(inst);
        return false;
}

void sreopengl_quit(sre_videodriver* video)
{
    sre_videoOpenGL* inst = video->userdata;

    SDL_GL_DeleteContext(inst->context);

    sre_delete(inst);
}

bool sreopengl_present(const sre_videodriver* video)
{
    SDL_ClearError();

    sre_videoOpenGL* inst = video->userdata;

    SDL_GL_SwapWindow(SDL_GL_GetCurrentWindow());

    return SDL_GetError()[0] != '\0';
}

bool sreopengl_viewport(const sre_videodriver* video, int w, int h) { glViewport(0, 0, w, h); return true; }
bool sreopengl_vsync(const sre_videodriver* video, int vsync) { return SDL_GL_SetSwapInterval(vsync) == 0; }

bool sreopengl_drawcleartest(const sre_videodriver* video, const sre_col4* color)
{
    GLclampf r = color->r / 255.0f;
    GLclampf g = color->g / 255.0f;
    GLclampf b = color->b / 255.0f;
    GLclampf a = color->a / 255.0f;

    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT);

    return true;
}


#define FN(x) (void*)x
bool voi() { return true; }

const struct sre_videodriverInterface sreopengl_interface = {
    sreopengl_quit,
    sreopengl_present,
    sreopengl_viewport,
    sreopengl_vsync,
    FN(voi),

    FN(voi),
    FN(voi),
    FN(voi),
    FN(voi),
    FN(voi),
    FN(voi),

    sreopengl_drawcleartest,
    FN(voi),
    FN(voi),
    FN(voi),
    FN(voi),
    FN(voi),
    FN(voi),
    FN(voi),
    FN(voi)
};