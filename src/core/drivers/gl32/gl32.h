#ifndef SRE_GL32_H
#define SRE_GL32_H

// OpenGL 3.2 driver header
// It'll be the main renderer for linux systems, and any other system that supports it, but isn't either Windows (for d3d11) or Apple (for Metal which will be added sometime)

#include <gl21/gl21.h>

struct sregl32_stateubo
{
    GLfloat viewport[16];
    GLfloat camera[2];
};

struct sregl32_drawdata
{
    GLuint program;
    GLuint vao;
    GLuint vbo;

    GLsizeiptr vbosize;
};

typedef struct sregl32_inst
{
    sregl21_cominst common21;
    struct sregl_functions glfuncs;
    struct sregl_functions21 glfuncs21;
    struct sregl_functions32 glfuncs32;

    struct sregl32_drawdata d1data;
    struct sregl32_drawdata d2data;

    GLuint stateubo;
    GLuint UBO_ALIGNMENT;
    GLuint UBO_STATEALIGN;
} sregl32_inst;

extern void sregl32_draw1(void* _inst, const sre_RenderInstance1* instances, size_t instance_count);
extern void sregl32_draw2(void* _inst, const sre_RenderPoint* points, size_t point_count, sre_draw2primitive mode);

extern void sregl32_begin(void* _inst, const float clear[4]);
extern void sregl32_end(void* _inst);

extern void sregl32_set_vsync(void* _inst, bool enable);
extern void sregl32_set_texturestate(void* _inst, void* _texture);
extern void sregl32_set_blendstate(void* _inst, sre_blendMode blendmode);
extern void sregl32_set_camerastate(void* _inst, sre_unit x, sre_unit y);
extern void sregl32_set_viewportstate(void* _inst, const sre_rect2Di* rectangle, sre_unit scale);
extern void sregl32_set_scissorstate(void* _inst, const sre_rect2Di* rectangle);

extern bool sregl32_texture_setup(void* _inst, void* _texture, sre_SDLpixelFormat formathint, int w, int h, sre_SDLpixelFormat* outformat);
extern bool sregl32_texture_update(void* _inst, void* _texture, const sre_rect2Di* region, const void* pixels, int pitch);
extern void sregl32_texture_destroy(void* _inst, void* _texture);

//

extern bool sregl32setupbuffers(sregl32_inst* instance);

#endif