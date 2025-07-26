#include <stdio.h>
#include <SDL.h>
#include "C/ExtraSDL.h"

int SDL_SetRenderDrawColorMod(SDL_Renderer* renderer, SDL_Color* color, SDL_Color* mod)
{
    if (*(Uint32*)mod == UINT32_MAX)
        return SDL_SetRenderDrawColor(renderer, color->r, color->g, color->b, color->a);
    else
        return SDL_SetRenderDrawColor(renderer,
            color->r * mod->r / 255.0f,
            color->g * mod->g / 255.0f,
            color->b * mod->b / 255.0f,
            color->a * mod->a / 255.0f
        );
}