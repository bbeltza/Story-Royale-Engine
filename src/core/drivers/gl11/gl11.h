#ifndef SRE_GL11_H
#define SRE_GL11_H

#include <glcommon/glcommon.h>

typedef struct sregl11_texture
{
    sregl_texture texture;
    float xrange;
    float yrange;
} sregl11_texture;

typedef struct sregl11_inst
{
    sregl_cominst common;

    struct sregl_functions glfuncs;
    struct sregl_functions11 glfuncs11;

    GLfloat scale_cache;
    
    sregl11_texture* curtexture;    
    bool hasARB_texture_non_power_of_two;
} sregl11_inst;

SRE_CAPI_BEGIN

extern void sregl11_draw1(void* _inst, const sre_RenderInstance1* instances, size_t instance_count);
extern void sregl11_draw2(void* _inst, const sre_RenderInstance2* instance, size_t point_count);

extern void sregl11_begin(void* _inst, const float clear[4]);
extern void sregl11_end(void* _inst);

extern void sregl11_set_viewportstate(void* _inst, int w, int h, sre_unit scale);
extern void sregl11_set_vsync(void* _inst, bool enable);
extern void sregl11_set_texturestate(void* _inst, void* _texture);
extern void sregl11_set_blendstate(void* _inst, sre_blendMode blendmode);
extern void sregl11_set_camerastate(void* _inst, sre_unit x, sre_unit y);
extern void sregl11_set_scissorstate(void* _inst, const sre_rect2Di* rectangle);

extern bool sregl11_texture_setup(void* _inst, void* _texture, sre_SDLpixelFormat formathint, int w, int h, sre_SDLpixelFormat* outformat);
extern bool sregl11_texture_update(void* _inst, void* _texture, const sre_rect2Di* region, const void* pixels, int pitch);
extern void sregl11_texture_destroy(void* _inst, void* _texture);

SRE_CAPI_END

#endif