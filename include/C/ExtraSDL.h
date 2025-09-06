#ifndef EXTRASDL_H
#define EXTRASDL_H

#include <SDL.h>
#include <SDL_ttf.h>

int SDL_RenderFillCircle(SDL_Renderer* renderer, int x, int y, int radius);
int SDL_RenderFillCircleF(SDL_Renderer* renderer, float x, float y, float radius);
int SDL_SetRenderDrawColorMod(SDL_Renderer* renderer, SDL_Color* color, SDL_Color* mod);

int TTF_MeasureTextSpaced(TTF_Font* font, const char* text, int measure_width, int* extent, int* count);

#endif // EXTRASDL_H