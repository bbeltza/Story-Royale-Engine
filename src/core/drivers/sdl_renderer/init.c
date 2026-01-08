#include <utils/logging.h>
#include "sdl_renderer.h"

int sresdlrenderer_init(sre_videodriver* video, SDL_Window *window)
{
    SDL_Renderer* const renderer = SDL_CreateRenderer(window, -1, 0);
    if (!renderer) return -1;

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    video->userdata = renderer;
    video->vsync = sresdlrenderer_vsync;
    video->present = sresdlrenderer_present;
    video->quit = sresdlrenderer_quit;

    video->draw_clear = sresdlrenderer_draw_clear;
    video->draw_fill = sresdlrenderer_draw_fill;
    video->draw_line = sresdlrenderer_draw_line;
    video->draw_lines = sresdlrenderer_draw_lines;
    video->draw_rect = sresdlrenderer_draw_rect;
    video->draw_rrect = sresdlrenderer_draw_rrect;
    video->draw_texture = sresdlrenderer_draw_texture;
    video->draw_rtexture = sresdlrenderer_draw_rtexture;
    return 0;
}

void sresdlrenderer_quit(sre_videodriver* video)
{
    SDL_DestroyRenderer(video->userdata);
}

int sresdlrenderer_vsync(const sre_videodriver* video, int vsync)
{
    return SDL_RenderSetVSync(video->userdata, vsync);
}

void sresdlrenderer_present(const sre_videodriver* video)
{
    SDL_RenderPresent(video->userdata);
}