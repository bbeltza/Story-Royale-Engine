#include "sdlrenderer.h"
#include <stdlib.h>

void sresdlrenderer_set_viewportstate(void* _inst, const sre_rect2Di* rectangle, sre_unit scale)
{
    sresdlrenderer_inst* inst = _inst;
    
    inst->scaling = scale;
    SDL_RenderSetViewport(inst->renderer, (const SDL_Rect*)rectangle);
}

void sresdlrenderer_set_texturestate(void* _inst, void* _texture)
{
    sresdlrenderer_inst* inst = _inst;
    sresdlrenderer_texture* texture = _texture;

    inst->cur_texture = texture ? texture->texture : NULL;
}

void sresdlrenderer_set_blendstate(void* _inst, sre_blendMode blending)
{
    sresdlrenderer_inst* inst = _inst;
    SDL_BlendMode mode;
    switch (blending)
    {
        //case SRE_BLEND_NONE: mode = SDL_BLENDMODE_NONE; break;
        case SRE_BLEND_BLEND: mode = SDL_BLENDMODE_BLEND; break;
        case SRE_BLEND_ADD: mode = SDL_BLENDMODE_ADD; break;
        case SRE_BLEND_MOD: mode = SDL_BLENDMODE_MOD; break;
        case SRE_BLEND_MUL: mode = SDL_BLENDMODE_MUL; break;
        default: abort();
    }

    SDL_SetRenderDrawBlendMode(inst->renderer, mode);
}

void sresdlrenderer_set_camerastate(void* _inst, sre_unit x, sre_unit y)
{
    sresdlrenderer_inst* inst = _inst;

    inst->camera = (sre_vec2ut){ x, y };
}

void sresdlrenderer_set_scissorstate(void* _inst, const sre_rect2Di* rectangle)
{
    sresdlrenderer_inst* inst = _inst;

    SDL_RenderSetClipRect(inst->renderer, (const SDL_Rect*)rectangle);
}

void sresdlrenderer_set_vsync(void* _inst, bool enable)
{
    sresdlrenderer_inst* inst = _inst;

    SDL_RenderSetVSync(inst->renderer, enable);
}