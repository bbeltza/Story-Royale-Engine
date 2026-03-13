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
    GLuint basic_program;
    
    GLuint basic_program_uniform_color;
    GLuint basic_program_uniform_model;
    GLuint basic_program_uniform_rotation;
    GLuint basic_program_uniform_anchor;
    GLuint basic_program_uniform_texture;
    GLuint basic_program_uniform_region;
    GLuint basic_program_uniform_flip;
    
    GLuint basic_program_state_projection;
    GLuint basic_program_state_cameraview;
    
    GLuint basicfill_program;
    GLuint basicfill_program_uniform_color;
    
    GLuint basic_texture;

    GLuint line_vbo;
    GLuint line_vao;

    

    GLfloat camera_view[16];
} sre_videoOpenGL;

extern const char* SRE_GL_FMTERR(GLenum err);
extern bool SRE_GL_LOAD2(struct sre_GLfuncs2_1* funcs); // Load OpenGL 2.1 functions (Minimum supported version)
extern bool SRE_GL_LOAD3(struct sre_GLfuncs3_2* inst); // Load OpenGL 3.2 functions (If supported)

#if defined(__FUNCSIG__)
    #define __PRETTY_FUNCTION__ __FUNCSIG__
#endif

#ifdef __cplusplus
    #define SRE_LOGERR sre::log<sre::LOGCATEGORY_ERROR>
#else
    #define SRE_LOGERR(...) sre_log(SRE_LOGCATEGORY_ERROR, __VA_ARGS__)
#endif

#define SRE_GL_CALL(x, ...) x;      \
while (1)                           \
{                                   \
    GLenum err = glGetError();      \
    if (err == GL_NO_ERROR) break;  \
    SRE_LOGERR(  \
        "[OPENGL] - (line: %d %s): Call to '" #x "' failed, error code %08x (%s)",     \
        __LINE__,                                                                      \
        __PRETTY_FUNCTION__,                                                           \
        err,                                                                                                \
        SRE_GL_FMTERR(err)                                                                                  \
    );                                                                                                      \
    __VA_ARGS__                                                                                             \
} (void)0

#define SRE_GL_CALL2_1(x, ...) SRE_GL_CALL(inst->funcs2##x, __VA_ARGS__)
#define SRE_GL_CALL3_2(x, ...) SRE_GL_CALL(inst->funcs3##x, __VA_ARGS__)

extern bool sreopengl_init(sre_videodriver* video, SDL_Window* window);

extern bool sreopengl_setupbuffers(sre_videoOpenGL* inst);

/* Drawing functions */

extern bool sreopengl_drawfill(const sre_videodriver* video, const sre_DDFill* data);
extern bool sreopengl_drawline(const sre_videodriver* video, const sre_DDLine* data);
extern bool sreopengl_drawlines(const sre_videodriver* video, const sre_DDLines* data);
extern bool sreopengl_drawrect(const sre_videodriver* video, const sre_DDRect* data);
extern bool sreopengl_drawrrect(const sre_videodriver* video, const sre_DDRRect* data);
extern bool sreopengl_drawtexture(const sre_videodriver* video, const sre_DDTexture* data);
extern bool sreopengl_drawrtexture(const sre_videodriver* video, const sre_DDRTexture* data);

//
/* Textures */

typedef struct sre_GLtexture
{
    GLuint id;
    int w;
    int h;
    SDL_PixelFormatEnum format;
} sre_GLtexture;

extern bool sreopengl_texcreate(const sre_videodriver* video, void* texture, int w, int h, SDL_PixelFormatEnum format);
extern bool sreopengl_texupdate(const sre_videodriver* video, void* texture, const void* pixels, int pitch);
extern void sreopengl_texdestroy(const sre_videodriver* video, void* texture);

extern bool sreopengl_texsize(const sre_videodriver* video, void* texture, int* w, int* h);
extern SDL_PixelFormatEnum sreopengl_texformat(const sre_videodriver* video, void* texture);
//

#ifndef IMGUI_DISABLE
    extern const struct sre_videodriverImGuiInterface sreopengl_imgui;
#endif

SRE_CAPI_END

#endif