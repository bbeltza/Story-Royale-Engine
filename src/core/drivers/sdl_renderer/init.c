#include <utils/logging.h>
#include "sdl_renderer.h"

sre_sdlrenderer sresdlrenderer_driver = {
    {
        .vsync = sresdlrenderer_vsync,
        .present = sresdlrenderer_present,

        .draw_fill = sresdlrenderer_draw_fill,
        .draw_line = sresdlrenderer_draw_line,
        .draw_lines = sresdlrenderer_draw_lines,
        .draw_rect = sresdlrenderer_draw_rect,
        .draw_rrect = sresdlrenderer_draw_rrect,
        .draw_texture = sresdlrenderer_draw_texture,
        .draw_rtexture = sresdlrenderer_draw_rtexture
    }
};

sre_videodriver* sresdlrenderer_init(SDL_Window *window)
{
    SDL_Renderer* const renderer = SDL_CreateRenderer(window, -1, 0);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    SDL_Texture* const rect_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB332, SDL_TEXTUREACCESS_STATIC, 1, 1);
    { uint8_t WHITE = 0xFF;
        SDL_UpdateTexture(rect_texture, NULL, &WHITE, 1);
    }

    sresdlrenderer_driver.renderer = renderer;
    sresdlrenderer_driver.rect_tex = rect_texture;

    return &sresdlrenderer_driver.video;
}

int sresdlrenderer_vsync(int vsync)
{
    return SDL_RenderSetVSync(sresdlrenderer_driver.renderer, vsync);
}

void sresdlrenderer_present()
{
    SDL_RenderPresent(sresdlrenderer_driver.renderer);
}