#ifndef SRE_GL32_H
#define SRE_GL32_H

// OpenGL 3.2 driver header
// It'll be the main renderer for linux systems, and any other system that supports it, but isn't either Windows (for d3d11) or Apple (for Metal which will be added sometime)

#include <gl21/gl21.h>
#define SREGL32_MAXINSTANCECOUNT 255

struct sregl32_stateubo
{
    GLfloat viewport[16];
    GLfloat camera[2];
    GLfloat _padding[2];
};

struct sregl32_d1data
{
    GLuint program;
    GLuint vao;
    GLuint vbo;
    
    GLuint instubo;
    GLuint instcolubo;
};

typedef struct sregl32_inst
{
    sregl21_cominst common21;
    struct sregl_functions glfuncs;
    struct sregl_functions21 glfuncs21;
    struct sregl_functions32 glfuncs32;

    struct sregl32_d1data d1data;

    GLuint stateubo;
} sregl32_inst;

void sregl32_flush_queueinstances1(void* inst, void* texture, const sre_RenderInstance1* instances, size_t instance_count, sre_u32 flags, sre_u32 switch_flags);
void sregl32_flush_queueinstances2(void* inst, void* texture, const sre_RenderInstance2* instance, size_t point_count, sre_u32 flags, sre_u32 switch_flags);

void sregl32_present(void* inst);
bool sregl32_clear(void* inst, float color[3]);

bool sregl32_set_viewportstate(void* inst, int w, int h, sre_unit scale);
bool sregl32_set_blendstate(void* inst, sre_blendMode blendmode);
bool sregl32_set_camerastate(void* inst, sre_unit x, sre_unit y);
void sregl32_set_clipstate(void* inst, const sre_rect2Di* rectangle);
void sregl32_set_vsync(void* inst, bool enable);

bool sregl32_texture_setup(void* inst, void* texture, sre_pixelFormat format, int w, int h, sre_pixelFormat* outformat);
bool sregl32_texture_update(void* inst, void* texture, const void* pixels, int pitch);
void sregl32_texture_destroy(void* inst, void* texture);

bool sregl32setupbuffers(sregl32_inst* instance);

#endif