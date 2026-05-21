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
    sresdlrenderer_inst* inst = _inst;
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
    sresdlrenderer_inst* inst = _inst;
    SDL_RenderPresent(inst->renderer);
}

void sresdlrenderer_flush_queueinstances1(void* _inst, void* _texture, const sre_RenderInstance1* instances, size_t instance_count, sre_u32 flags, sre_u32 switch_flags)
{
    (void)switch_flags;

    static const uint8_t DRAW1_INDICES[6] = {
        0, 1, 2,
        2, 3, 0
    };
    sresdlrenderer_inst* inst = _inst;
    sresdlrenderer_texture* texture = _texture;

    if (inst->vbuf_size < instance_count)
    {
        SDL_free(inst->vbuf);
        inst->vbuf_size += instance_count;
        inst->vbuf = SDL_malloc(inst->vbuf_size*4 * sizeof(SDL_Vertex));
        assert(inst->vbuf);
    }

    if (inst->ibuf_size < instance_count)
    {
        size_t old_size = inst->ibuf_size;
        inst->ibuf_size += instance_count;
        inst->ibuf = SDL_realloc(inst->ibuf, sizeof(*inst->ibuf)*inst->ibuf_size);
        assert(inst->ibuf);
        
        unsigned first_index = old_size ? inst->ibuf[old_size-1].r2 + 1 : 0;
        for (size_t i = old_size; i < inst->ibuf_size; i++)
        {
            inst->ibuf[i].l0 = first_index + 0;
            inst->ibuf[i].l1 = first_index + 1;
            inst->ibuf[i].l2 = first_index + 2;
            inst->ibuf[i].r0 = first_index + 1;
            inst->ibuf[i].r1 = first_index + 2;
            inst->ibuf[i].r2 = first_index + 3;
            first_index += 4;
        }
    }

    for (size_t i = 0; i < instance_count; i++)
    {
        #define Tround SDL_ceilf
        const sre_RenderInstance1* dinst = &instances[i];
        SDL_Vertex *positions = &inst->vbuf[i*4];

        sre_rect2Dut srect = {
            dinst->rectangle.x * inst->scaling,
            dinst->rectangle.y * inst->scaling,
            dinst->rectangle.w * inst->scaling,
            dinst->rectangle.h * inst->scaling
        };

        srect.x = srect.x + (flags & SRE_DRAWFLAG_CAMERAX ? inst->camera.x : 0);
        srect.y = srect.y + (flags & SRE_DRAWFLAG_CAMERAY ? inst->camera.y : 0);
        srect.w = srect.w;
        srect.h = srect.h;

        if (dinst->angle / 360 == ((int)dinst->angle) / 360 && 01)
        {
            positions[0].position.x = Tround(srect.x - dinst->anchor.x * srect.w);
            positions[0].position.y = Tround(srect.y - dinst->anchor.y * srect.h);

            positions[1].position.x = positions[0].position.x + srect.w,
            positions[1].position.y = positions[0].position.y;

            positions[2].position.x = positions[0].position.x;
            positions[2].position.y = positions[0].position.y + srect.h;

            positions[3].position.x = positions[0].position.x + srect.w;
            positions[3].position.y = positions[0].position.y + srect.h;
        }
        else
        {
            // We're going to multiply `space` by a rotation matrix, in which it is going to be offseted and scaled correctly later on
            /* ( c -s ) * (x)
               ( s  c ) * (y) */
            float space[4*2] = {
                -dinst->anchor.x, -dinst->anchor.y,
                space[0] + 1.0f, space[1],
                space[0],        space[1] + 1.0f,
                space[0] + 1.0f, space[1] + 1.0f
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

            for (int i = 0; i < 4; i++)
            {
                positions[i].position.x = srect.x + space[i*2 + 0];
                positions[i].position.y = srect.y + space[i*2 + 1];
            }
        }
        
        positions[0].tex_coord = (SDL_FPoint){ dinst->uv_offset.x, dinst->uv_offset.y };
        positions[1].tex_coord = (SDL_FPoint){ dinst->uv_offset.x + dinst->uv.x, dinst->uv_offset.y };
        positions[2].tex_coord = (SDL_FPoint){ dinst->uv_offset.x, dinst->uv_offset.y + dinst->uv.y };
        positions[3].tex_coord = (SDL_FPoint){ dinst->uv_offset.x + dinst->uv.x, dinst->uv_offset.y + dinst->uv.y };

        for (int i = 0; i < 4; i++)
            positions[i].color = *(SDL_Color*)&dinst->color;

        #if 0
            SDL_SetRenderDrawColor(inst->renderer, 255, 0, 0, 255);
            SDL_RenderDrawPointsF(inst->renderer, (SDL_FPoint*)positions, 4);
        #endif

    }
    
    int int_count = (int)instance_count;
    int res = SDL_RenderGeometry(inst->renderer, texture ? texture->texture : NULL, inst->vbuf, int_count*4, inst->ibuf->i, 6*int_count);
    assert(res == 0);
}

void sresdlrenderer_flush_queueinstances2(void* _inst, void* _texture, const sre_RenderInstance2* instance, size_t point_count, sre_u32 flags, sre_u32 switchflags)
{
    sresdlrenderer_inst* inst = _inst;
    sresdlrenderer_texture* texture = _texture;
    int res;

    sre_vec2f *vertices = SDL_stack_alloc(sre_vec2f, point_count);
    for (size_t i = 0; i < point_count; i++)
    {
        vertices[i].x = (instance->points[i].pos.x * inst->scaling) + (flags & SRE_DRAWFLAG_CAMERAX ? inst->camera.x : 0);
        vertices[i].y = (instance->points[i].pos.y * inst->scaling) + (flags & SRE_DRAWFLAG_CAMERAY ? inst->camera.y : 0);
    }

    switch (instance->mode)
    {
        case SRE_PRIMITIVE_TRIANGLES: {
            res = SDL_RenderGeometryRaw(
                inst->renderer,
                !texture ? NULL : texture->texture,
                &vertices->x, sizeof(sre_vec2f),
                (const SDL_Color*)&instance->color, 0,
                &instance->points->uv.x, sizeof(sre_RenderPoint),
                (int)point_count, NULL, 0, 0
            );
        } break;
        case SRE_PRIMITIVE_TRIANGLESTRIP: {
            assert(point_count >= 3);
            size_t indice_count = (point_count-3) * 3 + 3;
            uint16_t *indices = SDL_stack_alloc(uint16_t, indice_count); // 16-bit indices for now, enough for having a maximum of 65536 points c:
            indices[0] = 0;
            indices[1] = 1;
            indices[2] = 2;

            for (size_t i = 3; i < indice_count; i += 3)
            {
                indices[i+0] = instance->mode == SRE_PRIMITIVE_TRIANGLESTRIP ? (unsigned short)(i/3+0) : 0;
                indices[i+1] = (unsigned short)(i/3+1);
                indices[i+2] = (unsigned short)(i/3+2);
            }

            res = SDL_RenderGeometryRaw(
                inst->renderer,
                !texture ? NULL : texture->texture,
                &vertices->x, sizeof(sre_vec2f),
                (const SDL_Color*)&instance->color, 0,
                &instance->points->uv.x, sizeof(sre_RenderPoint),
                (int)point_count,
                indices, (int)indice_count, 2
            );
            SDL_stack_free(indices);
        } break;
        case SRE_PRIMITIVE_LINEPERLINE:
            res = 0;
            res += SDL_SetRenderDrawColor(inst->renderer, instance->color.r, instance->color.g, instance->color.b, instance->color.a);
            for (size_t i = 0; i < point_count; i += 2)
                res += SDL_RenderDrawLineF(inst->renderer,
                                            vertices[i+0].x, vertices[i+0].y,
                                            vertices[i+1].x, vertices[i+1].y
                                        );
            break;
        case SRE_PRIMITIVE_LINESTRIP:
            res = SDL_SetRenderDrawColor(inst->renderer, instance->color.r, instance->color.g, instance->color.b, instance->color.a);
            res = SDL_RenderDrawLinesF(inst->renderer, (SDL_FPoint*)vertices, (int)point_count);
            break;
        case SRE_PRIMITIVE_POINTS:
            res = SDL_SetRenderDrawColor(inst->renderer, instance->color.r, instance->color.g, instance->color.b, instance->color.a);
            res = SDL_RenderDrawPointsF(inst->renderer, (SDL_FPoint*)vertices, (int)point_count);
            break;
        default:
            assert(0 && "Not implemented or invalid (SRE_DRAW2_JOINED is not implemented)");
    }

    SDL_stack_free(vertices);

    assert(res == 0);
}
