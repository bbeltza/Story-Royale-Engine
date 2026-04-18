#ifndef SRE_GL11_H
#define SRE_GL11_H

#include <glcommon/glcommon.h>

typedef struct sregl11_inst
{
    sregl_cominst common;

    struct sregl_functions glfuncs;
    struct sregl_functions11 glfuncs11;

    GLfloat camera_cache[2];
    GLfloat scale_cache;
} sregl11_inst;

SRE_CAPI_BEGIN

extern void sregl11_flush_queueinstances1(void* _inst, void* _texture, const sre_RenderInstance1* instances, size_t instance_count, sre_u32 flags, sre_u32 switch_flags);
extern void sregl11_flush_queueinstances2(void* _inst, void* _texture, const sre_RenderInstance2* instance, size_t point_count, sre_u32 flags, sre_u32 switch_flags);

extern void sregl11_present(void* _inst);
extern bool sregl11_clear(void* _inst, float color[3]);

extern bool sregl11_set_viewportstate(void* _inst, int w, int h, sre_unit scale);
extern bool sregl11_set_blendstate(void* _inst, sre_blendMode blendmode);
extern bool sregl11_set_camerastate(void* _inst, sre_unit x, sre_unit y);
extern void sregl11_set_clipstate(void* _inst, const sre_rect2Di* rectangle);
extern void sregl11_set_vsync(void* _inst, bool enable);

extern bool sregl11_texture_setup(void* _inst, void* _texture, sre_pixelFormat formathint, int w, int h, sre_pixelFormat* outformat);
extern bool sregl11_texture_update(void* _inst, void* _texture, const void* pixels, int pitch);
extern void sregl11_texture_destroy(void* _inst, void* _texture);

SRE_CAPI_END

#endif