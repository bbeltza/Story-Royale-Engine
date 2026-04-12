#include "sdlrenderer.h"
#include <assert.h>

bool sresdlrenderer_clear(sresdlrenderer_interface* inst, float color[3])
{
    SDL_SetRenderDrawColor(inst->renderer,
        (Uint8)(color[0] * 255),
        (Uint8)(color[1] * 255),
        (Uint8)(color[2] * 255),
        SDL_ALPHA_OPAQUE
    );

    return SDL_RenderClear(inst->renderer) == 0;
}

void sresdlrenderer_present(sresdlrenderer_interface* inst)
{
    SDL_RenderPresent(inst->renderer);
}

void sresdlrenderer_flush_queueinstances1(sresdlrenderer_interface* inst, sre_Sampler*const* inst_textures, const sre_RenderInstance1* instances, size_t instance_count, sre_u32 flags)
{
    static const uint8_t DRAW1_INDICES[6] = {
        0, 1, 2,
        2, 3, 0
    };

    for (size_t i = 0; i < instance_count; i++)
    {
        #define Tround SDL_floorf
        const sre_RenderInstance1* dinst = &instances[i];
        sre_rect2Dut srect = {
            (dinst->rectangle.x + (flags & SRE_DRAWFLAG_CAMERA ? inst->camera.x : 0)) * inst->scaling,
            (dinst->rectangle.y + (flags & SRE_DRAWFLAG_CAMERA ? inst->camera.y : 0)) * inst->scaling,
            dinst->rectangle.w * inst->scaling,
            dinst->rectangle.h * inst->scaling
        };

        const float positions[4*2] = {
            Tround(srect.x - dinst->anchor.x * srect.w),
            Tround(srect.y - dinst->anchor.y * srect.h),

            positions[0] + srect.w,
            positions[1],

            positions[0] + srect.w,
            positions[1] + srect.h,
            
            positions[0],
            positions[1] + srect.h
        };

        const float uvs[4*2] = {
            dinst->uv_offset.x, dinst->uv_offset.y,
            dinst->uv_offset.x + dinst->uv.x, dinst->uv_offset.y,
            dinst->uv_offset.x + dinst->uv.x, dinst->uv_offset.y + dinst->uv.y,
            dinst->uv_offset.x, dinst->uv_offset.y + dinst->uv.y,
        };

        int res = SDL_RenderGeometryRaw(
            inst->renderer, inst_textures[i] ? inst_textures[i]->texture : NULL,
            positions, sizeof(float)*2,
            (SDL_Color*)&dinst->color, 0,
            uvs, sizeof(float)*2,
            4, DRAW1_INDICES, 6, 1
        );

        assert(res == 0);
            
    }
}

void sresdlrenderer_flush_queueinstances2(sresdlrenderer_interface* inst, const sre_RenderInstance2* instance, size_t point_count, sre_u32 flags)
{
    
}
