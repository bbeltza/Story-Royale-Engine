#include <string.h>
#include "C/ExtraSDL.h"

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

int TTF_MeasureTextSpaced(TTF_Font* font, const char* text, int measure_width, int* extent, int* count)
{
    size_t c;
    int res = TTF_MeasureText(font, text, measure_width, extent, &c);
    if (res < 0) goto ret;
    if (!count && !extent) goto ret;
    if (!count) count = &c; else *count = c;
    if (c >= strlen(text)) goto ret;

    char* spaced_text = malloc(*count+1);
    strncpy(spaced_text, text, *count);
    spaced_text[*count] = 0;

    char* spaced_ptr = strrchr(spaced_text, ' ');
    if (spaced_ptr && spaced_ptr != spaced_text)
        *spaced_ptr = 0;
    char* newline_ptr = strrchr(spaced_text, '\n');
    if (newline_ptr && newline_ptr != spaced_text)
        *newline_ptr = 0;
    
    res = TTF_MeasureText(font, spaced_text, measure_width, extent, count);

    free(spaced_text);
    ret:
    return res;
}