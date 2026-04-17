#include <SDL_video.h>
#include "gl32.h"

static void sregl32_destroy(void* _inst)
{

}

const struct sre_RenderVFT sregl32_vft = {
    sregl32_destroy
};

static bool sregl32_main(const struct sre_RenderVFT** vft, void* _inst, SDL_Window* window)
{
    return false;
}

const struct sre_RenderDriverData sregl32 = {
    .initialize = sregl32_main
};