#ifndef SRE_GL21_H
#define SRE_GL21_H

#include <assert.h>

#include <glcommon/glcommon.h>

struct sregl21_uniforms
{
    GLint viewport;
    GLint camera;
};

struct sregl21_vtassembler
{
    size_t size;
    sre_RenderPoint* arr;
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
} sregl21_inst;

SRE_CAPI_BEGIN

int sregl21_commonsetup(sregl21_cominst* inst, SDL_Window* window, struct sregl_functions* glfuncs, struct sregl_functions21* glfuncs21);
#define sregl21_commondestroy(inst) sregl_commondestroy(&(inst)->common)

//

extern void sregl21_draw1(void* _inst, const sre_RenderInstance1* instances, size_t instance_count);
extern void sregl21_draw2(void* _inst, const sre_RenderPoint* points, size_t point_count, sre_draw2primitive mode);

extern void sregl21_begin(void* _inst, const float clear[4]);
extern void sregl21_end(void* _inst);

extern void sregl21_set_viewportstate(void* _inst, int w, int h, sre_unit scale);
extern void sregl21_set_vsync(void* _inst, bool enable);
extern void sregl21_set_texturestate(void* _inst, void* _texture);
extern void sregl21_set_blendstate(void* _inst, sre_blendMode blendmode);
extern void sregl21_set_camerastate(void* _inst, sre_unit x, sre_unit y);
extern void sregl21_set_scissorstate(void* _inst, const sre_rect2Di* rectangle);

extern bool sregl21_texture_setup(void* _inst, void* _texture, sre_SDLpixelFormat formathint, int w, int h, sre_SDLpixelFormat* outformat);
extern bool sregl21_texture_update(void* _inst, void* _texture, const sre_rect2Di* region, const void* pixels, int pitch);
extern void sregl21_texture_destroy(void* _inst, void* _texture);

//

extern bool sregl21setupbuffers(sregl21_inst* instance);

extern bool sregl21_vtassemblersetup(sregl21_inst* inst, struct sregl21_vtassembler* vtasm, size_t initial_size);
extern void sregl21_vtassemblerreserve(sregl21_inst* inst, struct sregl21_vtassembler* vtasm, size_t size);
extern void sregl21_vtassemblerdata(sregl21_inst* inst, struct sregl21_vtassembler* vtasm, size_t size);
extern void sregl21_vtassemblerfree(struct sregl21_vtassembler* vtasm);

extern bool SREGL21_CHECKSHADER(struct sregl_functions21* funcs, GLuint shader, const char* tag);
extern bool SREGL21_CHECKPROGRAM(struct sregl_functions21* funcs, GLuint program, const char* tag);

SRE_CAPI_END

#endif