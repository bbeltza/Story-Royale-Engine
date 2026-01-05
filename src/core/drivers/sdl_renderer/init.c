#include <utils/logging.h>
#include "sdl_renderer.h"

sre_sdlrenderer sre_SDLRendererDriver;

sre_videodriver* sresdlrenderer_init(SDL_Window *window)
{
    LOG("HELLO DRIVERS");

    SDL_Renderer* const renderer = SDL_CreateRenderer(window, -1, 0);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    SDL_Texture* const rect_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB332, SDL_TEXTUREACCESS_STATIC, 1, 1);
    { uint8_t WHITE = 0xFF;
        SDL_UpdateTexture(rect_texture, NULL, &WHITE, 1);
    }

    sre_SDLRendererDriver.video.vsync = sresdlrenderer_vsync;

    sre_SDLRendererDriver.renderer = renderer;
    sre_SDLRendererDriver.rect_tex = rect_texture;

    return &sre_SDLRendererDriver.video;
}

int sresdlrenderer_vsync(int vsync)
{
    return SDL_RenderSetVSync(sre_SDLRendererDriver.renderer, vsync);
}