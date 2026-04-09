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

bool sresdlrenderer_set_blendstate(sresdlrenderer_interface* inst, sre_DrawBlending blending)
{
    return SDL_SetRenderDrawBlendMode(inst->renderer, blending) == 0;
}

bool sresdlrenderer_set_camerastate(sresdlrenderer_interface* inst, sre_unit x, sre_unit y)
{
    (void)inst;

    (void)x;
    (void)y;

    // Surely storing the state somewhere
    return true;
}