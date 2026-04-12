#include "sdlrenderer.h"
#include <assert.h>
#include <SDL_log.h>

#ifdef _INC_MALLOC
    #undef SDL_stack_alloc
    #undef SDL_stack_free
    #define SDL_stack_alloc(type, count) (type*)_malloca(count*sizeof(type))
    #define SDL_stack_free(block) _freea(block)
#endif

bool sresdlrenderer_clear(void* _inst, float color[3])
{
    sresdlrenderer_interface* inst = _inst;
    SDL_SetRenderDrawColor(inst->renderer,
        (Uint8)(color[0] * 255),
        (Uint8)(color[1] * 255),
        (Uint8)(color[2] * 255),
        SDL_ALPHA_OPAQUE
    );

    return SDL_RenderClear(inst->renderer) == 0;
}

void sresdlrenderer_present(void* _inst)
{
    sresdlrenderer_interface* inst = _inst;
    SDL_RenderPresent(inst->renderer);
}

void sresdlrenderer_flush_queueinstances1(void* _inst, sre_Sampler*const* inst_textures, const sre_RenderInstance1* instances, size_t instance_count, sre_u32 flags)
{
    static const uint8_t DRAW1_INDICES[6] = {
        0, 1, 2,
        2, 3, 0
    };
    sresdlrenderer_interface* inst = _inst;
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

        float positions[4*2];

        if (dinst->angle / 360 == ((int)dinst->angle) / 360 && 01)
        {
            positions[0] = Tround(srect.x - dinst->anchor.x * srect.w);
            positions[1] = Tround(srect.y - dinst->anchor.y * srect.h);

            positions[2] = positions[0] + srect.w,
            positions[3] = positions[1];

            positions[4] = positions[0] + srect.w;
            positions[5] = positions[1] + srect.h;
                
            positions[6] = positions[0];
            positions[7] = positions[1] + srect.h;
        }
        else
        {
            // We're going to multiply `space` by a rotation matrix, in which it is going to be offseted and scaled correctly later on
            /* ( c -s ) * (x)
               ( s  c ) * (y) */
            float space[4*2] = {
                -dinst->anchor.x, -dinst->anchor.y,
                space[0] + 1.0f, space[1],
                space[0] + 1.0f, space[1] + 1.0f,
                space[0],        space[1] + 1.0f
            };

            float s = SDL_sinf(dinst->angle);
            float c = SDL_cosf(dinst->angle);


            for (int i = 0; i < 4*2; i+=2)
            {
                float spacex = space[i+0] * srect.w;
                float spacey = space[i+1] * srect.h;

                space[i+0] = spacex * c - spacey * s;
                space[i+1] = spacex * s + spacey * c;
            }

            for (int i = 0; i < 4*2; i++)
            {
                positions[i] = (&srect.x)[i%2] + space[i];
            }
        }
        

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

        #if 0
            SDL_SetRenderDrawColor(inst->renderer, 255, 0, 0, 255);
            SDL_RenderDrawPointsF(inst->renderer, (SDL_FPoint*)positions, 4);
        #endif

        assert(res == 0);
            
    }
}

void sresdlrenderer_flush_queueinstances2(void* _inst, const sre_RenderInstance2* instance, size_t point_count, sre_u32 flags)
{
    sresdlrenderer_interface* inst = _inst;
    int res;

    sre_vec2f *vertices = SDL_stack_alloc(sre_vec2f, point_count);
    for (size_t i = 0; i < point_count; i++)
    {
        vertices[i].x = (instance->points[i].x + (flags & SRE_DRAWFLAG_CAMERA ? inst->camera.x : 0)) * inst->scaling;
        vertices[i].y = (instance->points[i].y + (flags & SRE_DRAWFLAG_CAMERA ? inst->camera.y : 0)) * inst->scaling;
    }

    switch (instance->mode)
    {
        case SRE_DRAW2_TRIANGLE: {
            res = SDL_RenderGeometryRaw(
                inst->renderer,
                NULL,
                &vertices->x, sizeof(sre_vec2f),
                (const SDL_Color*)&instance->color, 0,
                NULL, 0, point_count, NULL, 0, 0
            );
        } break;
        case SRE_DRAW2_JOINED: {
            assert(point_count >= 3);
            size_t indice_count = (point_count-3) * 3 + 3;
            uint16_t *indices = SDL_stack_alloc(uint16_t, indice_count); // 16-bit indices for now, enough for having a maximum of 65536 points c:
            indices[0] = 0;
            indices[1] = 1;
            indices[2] = 2;
            for (size_t i = 3; i < indice_count; i += 3)
            {
                indices[i+0] = (unsigned short)(i/3+0);
                indices[i+1] = (unsigned short)(i/3+1);
                indices[i+2] = (unsigned short)(i/3+2);
            }

            res = SDL_RenderGeometryRaw(
                inst->renderer,
                NULL,
                &vertices->x, sizeof(sre_vec2f),
                (const SDL_Color*)&instance->color, 0,
                NULL, 0, point_count,
                indices, indice_count, 2
            );
            SDL_stack_free(indices);
        } break;
        default:
            assert(0 && "Not implemented or invalid (SRE_DRAW2_JOINED is not implemented)");
    }

    SDL_stack_free(vertices);

    assert(res == 0);
}
