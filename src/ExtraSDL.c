#include "ExtraSDL.h"

int SDL_RenderFillCircle(SDL_Renderer* renderer, int x0, int y0, int radius)
{
    SDL_Rect v;
    SDL_RenderGetViewport(renderer, &v);

   if (x0 + radius < 0 || y0 + radius < 0 || x0 - radius > v.w || y0 - radius > v.h) return;

    int x = radius;
    int y = 0;
    int radiusError = 1 - x;
    while (x >= y) {
        SDL_RenderDrawLine(renderer, x + x0, y + y0, -x + x0, y + y0);
        SDL_RenderDrawLine(renderer, y + x0, x + y0, -y + x0, x + y0);
        SDL_RenderDrawLine(renderer, -x + x0, -y + y0, x + x0, -y + y0);
        SDL_RenderDrawLine(renderer, -y + x0, -x + y0, y + x0, -x + y0);
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