#include <Core/Texture.h>
#include <Core/Defer.h>

#include "drivers/drivers.h"
#include "../internal.h"

#include <utils/mem.h>

size_t sre_isinfreelist(sre_Texture id)
{
    for (size_t i = 0; i < engine.video->texture_flcount; i++)
        if (engine.video->texture_fl[i] == id) return i;
    return -1;
}

void* sre_get_texture(sre_Texture id)
{
    if (!id) return NULL;
    if (sre_isinfreelist(id) != -1) return NULL;

    id--;
    if (id > engine.video->textures_count) return NULL;
    return (char*)engine.video->textures + id * engine.video->texture_size;   
}

sre_Texture sre_tex_gen()
{
    SDL_LockMutex(engine.sdl_rendermutex);

    sre_Texture id = 0;
    for (size_t i = 0; i < engine.video->texture_flcount; i++)
    {
        if (!engine.video->texture_fl[i]) continue;

        id = engine.video->texture_fl[i];
        ((sre_Texture* const)engine.video->texture_fl)[i] = 0;
        break;
    }

    if (!id)
    {
        if (engine.video->textures_count >= engine.video->textures_capacity)
        {
            sre_usize new_capacity = engine.video->textures_capacity * 2;
            void* ptr = sre_newclear(new_capacity * engine.video->texture_size);

            ptr = memcpy(ptr, engine.video->textures, engine.video->textures_capacity * engine.video->texture_size);
            if (!ptr) goto FINISH_OR_FAIL;

            *(void**)&engine.video->textures = ptr;
            *(sre_usize*)&engine.video->textures_capacity = new_capacity;
        }

        id = (sre_Texture)(1 + engine.video->textures_count);
        (*(sre_usize*)&engine.video->textures_count)++;
    }

    if (engine.video->tex_gen(engine.video, (char*)engine.video->textures + (id - 1) * engine.video->texture_size) < 0)
    {
        sre_tex_destroy(id);
        id = 0;
    }

    FINISH_OR_FAIL:
    SDL_UnlockMutex(engine.sdl_rendermutex);
    return id;
}

void sre_tex_destroy(sre_Texture id)
{
    if (!id) return;
    if (!engine.video) return;
    size_t target = -1;

    SDL_LockMutex(engine.sdl_rendermutex);
    for (size_t i = 0; i < engine.video->texture_flcount; i++)
    {
        if (!engine.video->texture_fl[i]) target = i;
        else if (engine.video->texture_fl[i] == id) goto RET;
    }

    if (target == -1)
    {
        if (engine.video->texture_flcount >= engine.video->texture_flcapacity)
        {
            sre_usize new_capacity = engine.video->texture_flcapacity * 2;
            void* ptr = sre_newclear(new_capacity * sizeof(sre_Texture));

            ptr = memcpy(ptr, engine.video->texture_fl, engine.video->texture_flcapacity * sizeof(sre_Texture));
            assert(ptr);

            *(const sre_Texture**)&engine.video->texture_fl = ptr;
            *(sre_usize*)&engine.video->texture_flcapacity = new_capacity;
        }

        target = (int)engine.video->texture_flcount;
        (*(sre_usize*)&engine.video->texture_flcount)++;
    }

    engine.video->tex_destroy(engine.video, (char*)engine.video->textures + (id - 1) * engine.video->texture_size);

    RET:
    SDL_UnlockMutex(engine.sdl_rendermutex);
}

struct defer_texbind
{
    void* texture;
    const SDL_Surface* surface;
};

int deferred_texbind(struct defer_texbind* texbind) { return engine.video->tex_bind(engine.video, texbind->texture, texbind->surface); }

int sre_tex_bind(sre_Texture id, const SDL_Surface* surface)
{
    void* texture = sre_get_texture(id);
    if (!texture) return -1;

    struct defer_texbind texbind = {
        .texture = texture,
        .surface = surface
    };

    sre_sptr res = sre_defer_response((sre_deferResponseFunction)deferred_texbind, &texbind);
    return (int)res;
}

int sre_tex_update(sre_Texture id, const void* pixels, int pitch)
{
    void* texture = sre_get_texture(id);
    if (!texture) return -1;

    return engine.video->tex_update(engine.video, texture, pixels, pitch);
}

SDL_PixelFormatEnum sre_tex_format(sre_Texture id)
{
    void* texture = sre_get_texture(id);
    if (!texture) return SDL_PIXELFORMAT_UNKNOWN;

    return engine.video->tex_format(engine.video, texture);
}

int sre_tex_size(sre_Texture id, int* w, int* h)
{
    void* texture = sre_get_texture(id);
    if (!texture) return -1;

    return engine.video->tex_size(engine.video, texture, w, h);
}