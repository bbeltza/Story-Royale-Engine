#include "sdl_renderer.h"

static const struct sre_videodriverInterface sdl_interface = {
    sresdlrenderer_quit,
    sresdlrenderer_present,
    NULL,
    sresdlrenderer_vsync,
    sresdlrenderer_blend,

    sresdlrenderer_tex_gen,
    sresdlrenderer_tex_update,
    sresdlrenderer_tex_bind,
    sresdlrenderer_tex_size,
    sresdlrenderer_tex_destroy,
    sresdlrenderer_tex_format,

    sresdlrenderer_draw_clear,
    sresdlrenderer_clip,
    sresdlrenderer_draw_fill,
    sresdlrenderer_draw_line,
    sresdlrenderer_draw_lines,
    sresdlrenderer_draw_rect,
    sresdlrenderer_draw_rrect,
    sresdlrenderer_draw_texture,
    sresdlrenderer_draw_rtexture,
};

bool sresdlrenderer_init(sre_videodriver* video, SDL_Window *window)
{
    SDL_Renderer* const renderer = SDL_CreateRenderer(window, -1, 0);
    if (!renderer) return false;
    
    video->interface = &sdl_interface;
    video->userdata = renderer;
    video->texture_size = sizeof(SDL_Texture*);

#ifndef IMGUI_DISABLE
    video->imgui = &sresdlrenderer_imgui;
#endif
    return true;
}

bool sresdlrenderer_blend(const sre_videodriver* video, sre_DrawBlending blend)
{
    return SDL_SetRenderDrawBlendMode(video->userdata, (SDL_BlendMode)blend) == 0;
}

void sresdlrenderer_quit(sre_videodriver* video)
{
    SDL_DestroyRenderer(video->userdata);
}

bool sresdlrenderer_vsync(const sre_videodriver* video, int vsync)
{
    return SDL_RenderSetVSync(video->userdata, vsync) == 0;
}

void sresdlrenderer_present(const sre_videodriver* video)
{
    SDL_RenderPresent(video->userdata);
}