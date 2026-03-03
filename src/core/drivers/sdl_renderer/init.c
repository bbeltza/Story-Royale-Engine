#include "sdl_renderer.h"

bool sresdlrenderer_init(sre_videodriver* video, SDL_Window *window)
{
    SDL_Renderer* const renderer = SDL_CreateRenderer(window, -1, 0);
    if (!renderer) return false;

    video->userdata = renderer;
    video->vsync = sresdlrenderer_vsync;
    video->present = sresdlrenderer_present;
    video->quit = sresdlrenderer_quit;

    video->blend = sresdlrenderer_blend;

    video->draw_clear = sresdlrenderer_draw_clear;
    video->draw_fill = sresdlrenderer_draw_fill;
    video->draw_line = sresdlrenderer_draw_line;
    video->draw_lines = sresdlrenderer_draw_lines;
    video->draw_rect = sresdlrenderer_draw_rect;
    video->draw_rrect = sresdlrenderer_draw_rrect;
    video->draw_texture = sresdlrenderer_draw_texture;
    video->draw_rtexture = sresdlrenderer_draw_rtexture;

    video->tex_gen = sresdlrenderer_tex_gen;
    video->tex_bind = sresdlrenderer_tex_bind;
    video->tex_update = sresdlrenderer_tex_update;
    video->tex_size = sresdlrenderer_tex_size;
    video->tex_destroy = sresdlrenderer_tex_destroy;

    video->draw_clip = sresdlrenderer_clip;

    video->texture_size = sizeof(SDL_Texture*);
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