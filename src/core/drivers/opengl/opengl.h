#ifndef SRE_DRIVER_OPENGL_H
#define SRE_DRIVER_OPENGL_H
#include <drivers.h>
#include <C_API.h>

#include <SDL_opengl.h>
#include <Base/Log.h>

#include "functions.h"

SRE_CAPI_BEGIN

struct sre_GLfuncs2_1
{
    SRE_GL_FUNCTIONS_2_1
};

struct sre_GLfuncs3_2
{
    SRE_GL_FUNCTIONS_3_2
};

typedef struct sre_videoOpenGL
{
    SDL_GLContext context;
    struct sre_GLfuncs2_1 funcs2;
    struct sre_GLfuncs3_2 funcs3;

    // Basic vertex object buffers
    GLuint basic_vao; // Not using VAOs for now, since they're a 3.0 feature (I know kind of weird)
    GLuint basic_vbo;
    GLuint basic_ibo;
} sre_videoOpenGL;

typedef struct sre_GLtexture
{
    void* happy;
} sre_GLtexture;

extern const char* SRE_GL_FMTERR(GLenum err);
extern bool SRE_GL_LOAD2(struct sre_GLfuncs2_1* funcs); // Load OpenGL 2.1 functions (Minimum supported version)
extern bool SRE_GL_LOAD3(struct sre_GLfuncs3_2* inst); // Load OpenGL 3.2 functions (If supported)
#ifndef __PRETTY_FUNCTION__
    #define __PRETTY_FUNCTION__ __FUNCSIG__
#endif
#define SRE_GL_CALL(x, ...) x;      \
while (1)                           \
{                                   \
    GLenum err = glGetError();      \
    if (err == GL_NO_ERROR) break;  \
    sre_log(SRE_LOGCATEGORY_ERROR,  \
        "[OPENGL] - (line: %d " __PRETTY_FUNCTION__ "): Call to '" #x "' failed, error code %08x (%s)",     \
        __LINE__,                                                                                           \
        err,                                                                                                \
        SRE_GL_FMTERR(err)                                                                                  \
    );                                                                                                      \
    __VA_ARGS__                                                                                             \
} (void)0

#define SRE_GL_CALL2_1(x, ...) SRE_GL_CALL(inst->funcs2##x, __VA_ARGS__)
#define SRE_GL_CALL3_2(x, ...) SRE_GL_CALL(inst->funcs3##x, __VA_ARGS__)

extern bool sreopengl_init(sre_videodriver* video, SDL_Window* window);

extern bool sreopengl_setupbuffers(sre_videoOpenGL* inst);

#ifndef IMGUI_DISABLE
    extern const struct sre_videodriverImGuiInterface sreopengl_imgui;
#endif

SRE_CAPI_END

#endif