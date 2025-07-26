#ifndef EXTRASDL_H
#define EXTRASDL_H

#include <SDL.h>
#include <SDL_ttf.h>

int SDL_RenderFillCircle(SDL_Renderer* renderer, int x0, int y0, int radius);
int TTF_MeasureTextSpaced(TTF_Font* font, const char* text, int measure_width, int* extent, int* count);
int SDL_SetRenderDrawColorMod(SDL_Renderer* renderer, SDL_Color* color, SDL_Color* mod);

#endif // EXTRASDL_H