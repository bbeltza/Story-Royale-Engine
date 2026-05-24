#ifndef SRE_GL21_H
#define SRE_GL21_H

#include <assert.h>

#include <glcommon/glcommon.h>

typedef struct sregl21_vertex
{
    sre_vec2f pos;
    sre_vec2f uv;
    sre_col4 col;
} sregl21_vertex;

struct sregl21_uniforms
{
    GLint viewport;
    GLint camera;
};

struct sregl21_vtassembler
{
    size_t size;
    sregl21_vertex* arr;
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
    GLuint mainprogram;
    GLuint mainvbo;
    size_t mainvbosize;
    struct sregl21_uniforms uniforms;
    struct sregl21_vtassembler vtassembler;

    struct sregl21_drawcache cache;
} sregl21_inst;

SRE_CAPI_BEGIN

int sregl21_commonsetup(sregl21_cominst* inst, SDL_Window* window, struct sregl_functions* glfuncs, struct sregl_functions21* glfuncs21);
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
bool sregl21_texture_update(void* inst, void* texture, const sre_rect2Di* region, const void* pixels, int pitch);
void sregl21_texture_destroy(void* inst, void* texture);

bool sregl21setupbuffers(sregl21_inst* instance);

bool sregl21_vtassemblersetup(sregl21_inst* inst, struct sregl21_vtassembler* vtasm, size_t initial_size);
void sregl21_vtassemblerreserve(sregl21_inst* inst, struct sregl21_vtassembler* vtasm, size_t size);
void sregl21_vtassemblerdata(sregl21_inst* inst, struct sregl21_vtassembler* vtasm, size_t size);
void sregl21_vtassemblerfree(struct sregl21_vtassembler* vtasm);

bool SREGL21_CHECKSHADER(struct sregl_functions21* funcs, GLuint shader, const char* tag);
bool SREGL21_CHECKPROGRAM(struct sregl_functions21* funcs, GLuint program, const char* tag);

SRE_CAPI_END

#endif