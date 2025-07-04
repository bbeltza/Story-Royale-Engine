#ifndef COL_FUNC_H
#define COL_FUNC_H

#include "datatypes/Color.h"


#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

    typedef struct SDL_Renderer SDL_Renderer;
    int SDL_SetRenderDrawColorMod(SDL_Renderer* renderer, Color4* color, Color4* mod);

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // !COL_FUNC_H