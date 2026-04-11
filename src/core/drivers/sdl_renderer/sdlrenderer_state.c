#include "sdlrenderer.h"

bool sresdlrenderer_set_viewportstate(sresdlrenderer_interface* inst, int w, int h, sre_unit scale)
{
    (void)inst;

    (void)w;
    (void)h;
    (void)scale;
    // Do nothing for now, I may store these variables though

    return true;
}

bool sresdlrenderer_set_blendstate(sresdlrenderer_interface* inst, sre_blendMode blending)
{
    SDL_BlendMode mode;
    switch (blending)
    {
        case SRE_BLEND_NONE: mode = SDL_BLENDMODE_NONE; break;
        case SRE_BLEND_BLEND: mode = SDL_BLENDMODE_BLEND; break;
        case SRE_BLEND_ADD: mode = SDL_BLENDMODE_ADD; break;
        case SRE_BLEND_MOD: mode = SDL_BLENDMODE_MOD; break;
        case SRE_BLEND_MUL: mode = SDL_BLENDMODE_MUL; break;
        default: return false;
    }

    return SDL_SetRenderDrawBlendMode(inst->renderer, mode) == 0;
}

bool sresdlrenderer_set_camerastate(sresdlrenderer_interface* inst, sre_unit x, sre_unit y)
{
    (void)inst;

    (void)x;
    (void)y;

    // Surely storing the state somewhere
    return true;
}

void sresdlrenderer_set_clipstate(sresdlrenderer_interface* inst, const sre_rect2Di* rectangle)
{
    SDL_RenderSetClipRect(inst->renderer, (const SDL_Rect*)rectangle);
}

void sresdlrenderer_set_vsync(sresdlrenderer_interface* inst, bool enable)
{
    SDL_RenderSetVSync(inst->renderer, enable);
}