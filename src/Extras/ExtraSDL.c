#include "ExtraSDL.h"
#include <string.h>

int SDL_RenderFillCircle(SDL_Renderer* renderer, int x0, int y0, int radius)
{
    SDL_Rect v;
    SDL_RenderGetViewport(renderer, &v);

   if (x0 + radius < 0 || y0 + radius < 0 || x0 - radius > v.w || y0 - radius > v.h) return 1;

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

int TTF_MeasureTextSpaced(TTF_Font* font, const char* text, int measure_width, int* extent, int* count)
{
    int c;
    int res = TTF_MeasureText(font, text, measure_width, extent, &c);
    if (res < 0) goto ret;
    if (!count && !extent) goto ret;
    if (!count) count = &c; else *count = c;
    if (*count >= strlen(text)) goto ret;

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