#ifndef SRE_DRIVER_SDLRENDERER_H
#define SRE_DRIVER_SDLRENDERER_H
#include <drivers.h>
#include <C_API.h>

SRE_CAPI_BEGIN

extern int sresdlrenderer_init(sre_videodriver* video, SDL_Window* window);
extern void sresdlrenderer_quit(sre_videodriver* video);

extern int sresdlrenderer_vsync(const sre_videodriver* video, int vsync);
extern void sresdlrenderer_present(const sre_videodriver* video);

extern int sresdlrenderer_draw_clear(const sre_videodriver* video, const sre_col4* color);
extern int sresdlrenderer_draw_fill(const sre_videodriver* video, const sre_DDFill* data);
extern int sresdlrenderer_draw_line(const sre_videodriver* video, const sre_DDLine* data);
extern int sresdlrenderer_draw_lines(const sre_videodriver* video, const sre_DDLines* data);
extern int sresdlrenderer_draw_rect(const sre_videodriver* video, const sre_DDRect* data);
extern int sresdlrenderer_draw_rrect(const sre_videodriver* video, const sre_DDRRect* data);
extern int sresdlrenderer_draw_texture(const sre_videodriver* video, const sre_DDTexture* data);
extern int sresdlrenderer_draw_rtexture(const sre_videodriver* video, const sre_DDRTexture* data);

extern int sresdlrenderer_tex_gen(const sre_videodriver* video, void* texture);
extern int sresdlrenderer_tex_update(const sre_videodriver* video, void* texture, const void* pixels, int pitch);
extern int sresdlrenderer_tex_bind(const sre_videodriver* video, void* texture, const SDL_Surface* surface);
extern int sresdlrenderer_tex_size(const sre_videodriver* video, void* texture, int* w, int* h);
extern SDL_PixelFormatEnum sresdlrenderer_tex_format(const sre_videodriver* video, void* texture);
extern void sresdlrenderer_tex_destroy(const sre_videodriver* video, void* texture);

extern int sresdlrenderer_clip(const sre_videodriver* video, const sre_rect2Dut* rect);

SRE_CAPI_END

#endif