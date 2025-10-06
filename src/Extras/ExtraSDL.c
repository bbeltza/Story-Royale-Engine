#include <string.h>
#include <stdlib.h>
#include "C/ExtraSDL.h"

int SDL_SetRenderDrawColorMod(SDL_Renderer* renderer, SDL_Color* color, SDL_Color* mod)
{
    if (*(Uint32*)mod == UINT32_MAX)
        return SDL_SetRenderDrawColor(renderer, color->r, color->g, color->b, color->a);
    else
        return SDL_SetRenderDrawColor(renderer,
            (Uint8)(color->r * mod->r / 255.0f),
            (Uint8)(color->g * mod->g / 255.0f),
            (Uint8)(color->b * mod->b / 255.0f),
            (Uint8)(color->a * mod->a / 255.0f)
        );
}

int SDL_RenderFillCircleF(SDL_Renderer* renderer, float _x, float _y, float radius)
{
    SDL_Rect v;
    SDL_RenderGetViewport(renderer, &v);

   if (_x + radius < 0 || _y + radius < 0 || _x - radius > v.w || _y - radius > v.h) return 1;

    float x = radius;
    float y = 0;
    float radiusError = 1 - x;
    while (x >= y) {
        SDL_RenderDrawLineF(renderer, x + _x, y + _y, -x + _x, y + _y);
        SDL_RenderDrawLineF(renderer, y + _x, x + _y, -y + _x, x + _y);
        SDL_RenderDrawLineF(renderer, -x + _x, -y + _y, x + _x, -y + _y);
        SDL_RenderDrawLineF(renderer, -y + _x, -x + _y, y + _x, -x + _y);
        y++;
        if (radiusError < 0)
            radiusError += 2 * y + 1;
        else {
            x--;
            radiusError += 2 * (y - x + 1);
        }
    }

    return 1;
}

int SDL_RenderFillCircle(SDL_Renderer* renderer, int _x, int _y, int radius)
{
    return SDL_RenderFillCircleF(renderer, (float)_x, (float)_y, (float)radius);
}