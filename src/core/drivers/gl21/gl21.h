#ifndef SRE_GL21_H
#define SRE_GL21_H

#include <Core/Render.h>
#include <SDL_video.h>
#include <assert.h>

#include <glcommon/glcommon.h>

struct sre_Sampler
{
    int happy;
};

// Common shader uniform names (shared between draw1 and draw2)
struct sregl21_csu
{
    GLint viewport;
    GLint camera;
    GLint color; // unsigned int vector containing the raw color data (note that color will be converted from 1 byte to 4 bytes each component)
};

struct sregl21_d1su
{
    GLint model; // 4x4 "matrix" containing all of the draw1 state (position, size, anchor and rotation ; Color is already in the common data)
                    // It's not a model matrix, the shader will still have to rearrange the data to make it work
};

// Dependencies structure for draw1
struct sregl21_draw1dep
{
    GLuint program;
    GLuint vbo;
    GLuint ibo;

    struct sregl21_csu common_uniforms;
    struct sregl21_d1su depend_uniforms;
};

struct sregl21_draw2dep
{
    GLuint program;
    GLuint vbo;
    GLsizei bufsize;

    struct sregl21_csu common_uniforms;
};

typedef struct sregl21_inst
{
    sre_RenderInterface interface;
    
    struct sregl_functions glfuncs;
    struct sregl_functions21 glfuncs21;

    SDL_GLContext context;
    SDL_Window* window;

    // Common uniforms
    struct sregl21_draw1dep draw1data;
    struct sregl21_draw2dep draw2data;
} sregl21_inst;

SRE_CAPI_BEGIN

void sregl21_flush_queueinstances1(void* inst, sre_Sampler*const* inst_textures, const sre_RenderInstance1* instances, size_t instance_count, sre_u32 flags);
void sregl21_flush_queueinstances2(void* inst, const sre_RenderInstance2* instance, size_t point_count, sre_u32 flags);

void sregl21_present(void* inst);
bool sregl21_clear(void* inst, float color[3]);

bool sregl21_set_viewportstate(void* inst, int w, int h, sre_unit scale);
bool sregl21_set_blendstate(void* inst, sre_blendMode blending);
bool sregl21_set_camerastate(void* inst, sre_unit x, sre_unit y);
void sregl21_set_clipstate(void* inst, const sre_rect2Di* rectangle);
void sregl21_set_vsync(void* inst, bool enable);

bool sregl21_setup_texture(void* inst, sre_Sampler* texture, sre_pixelFormat format, int w, int h);
bool sregl21_update_texture(void* inst, sre_Sampler* texture, const void* pixels, int pitch);
bool sregl21_query_texture(void* inst, sre_Sampler* texture, int size[2], sre_pixelFormat* format);
void sregl21_destroy_texture(void* inst, sre_Sampler* texture);

SRE_CAPI_END

#endif