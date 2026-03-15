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

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS,
    #ifndef NDEBUG
        SDL_GL_CONTEXT_DEBUG_FLAG
    #else
        0
    #endif
    );

    Uint32 win_flags = SDL_GetWindowFlags(window);
    if (!(win_flags & SDL_WINDOW_OPENGL))
    {
        win_flags |= SDL_WINDOW_OPENGL;
        if (SDL_RecreateWindow(window, win_flags) < 0)
            return false;
    }

    sre_videoOpenGL* inst = sre_newclear(sizeof(sre_videoOpenGL));

#if 01
   // Chose core 3.2 first, switch to compat 2.1 if it's not available
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    inst->context = SDL_GL_CreateContext(window);
#endif
    if (!inst->context)
    {
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
        inst->context = SDL_GL_CreateContext(window);
        if (!inst->context)
            goto FAIL;
    }

    video->userdata = inst;
    video->interface = &sreopengl_interface;
    video->texture_size = sizeof(sre_GLtexture);
    #ifndef IMGUI_DISABLE
        video->imgui = &sreopengl_imgui;
    #endif

    if (!SRE_GL_LOAD2(&inst->funcs2))
        goto FAIL;
    if (!SRE_GL_LOAD3(&inst->funcs3))
        goto FAIL;

    if (!sreopengl_setupbuffers(inst))
        goto FAIL;

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

void sreopengl_present(const sre_videodriver* video)
{
    //sre_videoOpenGL* inst = video->userdata;

    SDL_GL_SwapWindow(SDL_GL_GetCurrentWindow());
}

bool sreopengl_vsync(const sre_videodriver* video, int vsync) { return SDL_GL_SetSwapInterval(vsync) == 0; }
bool sreopengl_viewport(const sre_videodriver* video, int w, int h)
{
    const sre_videoOpenGL* inst = video->userdata;

    GLfloat right = (GLfloat)w;
    GLfloat bottom = (GLfloat)h;
    GLfloat projection[16] = {
        2 / right, 0, 0, 0,
        0, -2 / bottom, 0, 0,
        0, 0,           video->scale, 0,
        -1, 1,          0, 1
    };

    SRE_GL_CALL(inst->funcs2.glUseProgram(inst->line_program));
    SRE_GL_CALL(inst->funcs2.glUniformMatrix4fv(inst->line_program_uniform_projection, 1, GL_FALSE, projection));

    SRE_GL_CALL(inst->funcs2.glUseProgram(inst->basic_program));
    SRE_GL_CALL(inst->funcs2.glUniformMatrix4fv(inst->basic_program_state_projection, 1, GL_FALSE, projection));
    SRE_GL_CALL(glViewport(0, 0, w, h));
    return true;
}

bool sreopengl_blend(const sre_videodriver* video, sre_DrawBlending blending)
{
    if (blending == SRE_BLEND_NONE)
    {
        SRE_GL_CALL(glDisable(GL_BLEND), return false;);
        return true;
    }

    GLenum sfactor;
    GLenum dfactor;
    switch (blending)
    {
        case SRE_BLEND_BLEND:
            sfactor = GL_SRC_ALPHA;
            dfactor = GL_ONE_MINUS_SRC_ALPHA;
            break;
        case SRE_BLEND_ADD:
            sfactor = GL_SRC_ALPHA;
            dfactor = GL_ONE;
            break;
        case SRE_BLEND_MOD:
            sfactor = GL_DST_COLOR;
            dfactor = GL_ZERO;
            break;
        case SRE_BLEND_MUL:
            sfactor = GL_DST_COLOR;
            dfactor = GL_ONE_MINUS_SRC_ALPHA;
            break;
        default: abort();
    }

    SRE_GL_CALL(glEnable(GL_BLEND), return false;);
    SRE_GL_CALL(glBlendFunc(sfactor, dfactor), return false;);
    return true;
}

bool sreopengl_drawcleartest(const sre_videodriver* video, const sre_col4* color)
{
    GLclampf r = color->r / 255.0f;
    GLclampf g = color->g / 255.0f;
    GLclampf b = color->b / 255.0f;
    GLclampf a = color->a / 255.0f;

    SRE_GL_CALL(glClearColor(r, g, b, a));
    SRE_GL_CALL(glClear(GL_COLOR_BUFFER_BIT));

    sre_videoOpenGL* inst = video->userdata;

    const GLfloat tx = video->center.x - video->camera.x;
    const GLfloat ty = video->center.y - video->camera.y;

    //sre_log(SRE_LOGCATEGORY_DEBUG, "%f %f", tx, ty);

    memcpy(inst->camera_view, (GLfloat[16]){
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        tx,   ty,   0.0f, 1.0f
    }, sizeof(GLfloat[16]));
    return true;
}

bool sreopengl_clip(const sre_videodriver* video, const sre_rect2Dut* rect)
{
    if (!rect)
    {
        SRE_GL_CALL(glDisable(GL_SCISSOR_TEST), return false;);
        return true;
    }

    SRE_GL_CALL(glEnable(GL_SCISSOR_TEST), return false;);

    sre_rect2Di r = {
        (GLint)(rect->x * video->scale),
        (GLint)((video->size.y - rect->y) * video->scale),
        (GLint)(rect->w * video->scale),
        (GLint)(rect->h * video->scale)
    };
    SRE_GL_CALL(glScissor(r.x, r.y, r.w, r.h), return false;);
    return true;
}


#define FN(x) (void*)x
long voi() { return 1; }
long fai() { return 0; }

const struct sre_videodriverInterface sreopengl_interface = {
    sreopengl_quit,
    sreopengl_present,
    sreopengl_viewport,
    sreopengl_vsync,
    sreopengl_blend,

    sreopengl_texcreate,
    sreopengl_texupdate,
    sreopengl_texdestroy,
    sreopengl_texsize,
    sreopengl_texformat,

    sreopengl_drawcleartest,
    sreopengl_clip,
    sreopengl_drawfill,
    sreopengl_drawline,
    sreopengl_drawlines,
    sreopengl_drawrect,
    sreopengl_drawrrect,
    sreopengl_drawtexture,
    sreopengl_drawrtexture
};