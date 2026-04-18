#ifndef SRE_GL21_H
#define SRE_GL21_H

#include <assert.h>

#include <glcommon/glcommon.h>

// Common shader uniform names (shared between draw1 and draw2)
struct sregl21_csu
{
    GLint viewport;
    GLint camera;
    GLint color; // unsigned int vector containing the raw color data (note that color will be converted from 1 byte to 4 bytes each component)
    GLint sampler; // Unused by draw2. Note that draw2 is planned to support textures pretty soon
                    // Also note that the fragment shader uses it so it has to still be bound (it is now, to the basic texture)
};

struct sregl21_d1su
{
    GLint model; // 4x3 "matrix" containing all of the draw1 state (position, size, anchor and rotation ; Color is already in the common data)
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

struct sregl21_drawcache
{
    GLfloat camera[2];
};

// Data that's shared accross both OpenGL 2.1 and 3.2
typedef struct sregl21_cominst
{
    sregl_cominst common;

    GLint basic_texture;
} sregl21_cominst;

typedef struct sregl21_inst
{
    sregl21_cominst common21;
    struct sregl_functions glfuncs;
    struct sregl_functions21 glfuncs21;

    // Draw dependencies
    struct sregl21_draw1dep draw1data;
    struct sregl21_draw2dep draw2data;

    struct sregl21_drawcache cache;
} sregl21_inst;

SRE_CAPI_BEGIN

bool sregl21_commonsetup(sregl21_cominst* inst, SDL_Window* window, struct sregl_functions* glfuncs, struct sregl_functions21* glfuncs21);
#define sregl21_commondestroy(inst) sregl_commondestroy(&(inst)->common)

//

void sregl21_flush_queueinstances1(void* inst, void* texture, const sre_RenderInstance1* instances, size_t instance_count, sre_u32 flags, sre_u32 switch_flags);
void sregl21_flush_queueinstances2(void* inst, void* texture, const sre_RenderInstance2* instance, size_t point_count, sre_u32 flags, sre_u32 switch_flags);

void sregl21_present(void* inst);
bool sregl21_clear(void* inst, float color[3]);

bool sregl21_set_viewportstate(void* inst, int w, int h, sre_unit scale);
bool sregl21_set_blendstate(void* inst, sre_blendMode blending);
bool sregl21_set_camerastate(void* inst, sre_unit x, sre_unit y);
void sregl21_set_clipstate(void* inst, const sre_rect2Di* rectangle);
void sregl21_set_vsync(void* inst, bool enable);

bool sregl21_texture_setup(void* inst, void* texture, sre_pixelFormat format, int w, int h, sre_pixelFormat* outformat);
bool sregl21_texture_update(void* inst, void* texture, const void* pixels, int pitch);
void sregl21_texture_destroy(void* inst, void* texture);

bool sregl21setupbuffers(sregl21_inst* instance);
void sregl21bindbuffer1(sregl21_inst* instance, GLint vbo);
void sregl21bindbuffer2(sregl21_inst* instance, GLint vbo);

bool SREGL21_CHECKSHADER(struct sregl_functions21* funcs, GLuint shader, const char* tag);
bool SREGL21_CHECKPROGRAM(struct sregl_functions21* funcs, GLuint program, const char* tag);

SRE_CAPI_END

#endif