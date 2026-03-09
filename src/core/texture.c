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

struct defer_texcreate
{
    void* texture;
    int w;
    int h;
    SDL_PixelFormatEnum format;
};

struct defer_texupdate
{
    void* texture;
    const void* pixels;
    const int pitch;
};

sre_sptr deferred_texcreate(struct defer_texcreate* data)
{
    return engine.video->interface->tex_create(engine.video,
        data->texture,
        data->w,
        data->h,
        data->format
    );
}

sre_sptr deferred_texupdate(struct defer_texupdate* data)
{
    return engine.video->interface->tex_update(engine.video,
        data->texture,
        data->pixels,
        data->pitch
    );
}

sre_Texture sre_tex_create(int w, int h, SDL_PixelFormatEnum format)
{
    sre_Texture id = 0;
    for (size_t i = 0; i < engine.video->texture_flcount; i++)
    {
        if (!engine.video->texture_fl[i]) continue;

        id = engine.video->texture_fl[i];
        engine.video->texture_fl[i] = 0;
        break;
    }

    if (!id)
    {
        if (engine.video->textures_count >= engine.video->textures_capacity)
        {
            size_t new_capacity = engine.video->textures_capacity * 2;
            void* ptr = sre_newclear(new_capacity * engine.video->texture_size);

            ptr = memcpy(ptr, engine.video->textures, engine.video->textures_capacity * engine.video->texture_size);
            if (!ptr) goto FAIL;

            sre_delete(engine.video->textures);
            engine.video->textures = ptr;
            engine.video->textures_capacity = new_capacity;
        }

        id = (sre_Texture)(1 + engine.video->textures_count);
        engine.video->textures_count++;
    }

    struct defer_texcreate data = {
        .texture = (char*)engine.video->textures + (id - 1) * engine.video->texture_size,
        .w = w,
        .h = h,
        .format = format
    };

    if (!sre_defer_response((sre_deferResponseFunction)deferred_texcreate, &data))
        goto FAIL;

    return id;

    FAIL:
    sre_tex_destroy(id);
    return 0;
}

bool sre_tex_update(sre_Texture id, const void* pixels, int pitch)
{
    void* texture = sre_get_texture(id);
    if (!texture) return false;

    struct defer_texupdate data = {
        .texture = texture,
        .pixels = pixels,
        .pitch = pitch
    };

    return sre_defer_response((sre_deferResponseFunction)deferred_texupdate, &data);
}

void deferred_texdestroy(void* texture) { engine.video->interface->tex_destroy(engine.video, texture); }
void sre_tex_destroy(sre_Texture id)
{
    if (!id) return;
    if (!engine.video) return;
    size_t target = -1;

    for (size_t i = 0; i < engine.video->texture_flcount; i++)
    {
        if (!engine.video->texture_fl[i]) target = i;
        else if (engine.video->texture_fl[i] == id) return;
    }

    if (target == -1)
    {
        if (engine.video->texture_flcount >= engine.video->texture_flcapacity)
        {
            size_t new_capacity = engine.video->texture_flcapacity * 2;
            void* ptr = sre_newclear(new_capacity * sizeof(sre_Texture));

            ptr = memcpy(ptr, engine.video->texture_fl, engine.video->texture_flcapacity * sizeof(sre_Texture));
            assert(ptr);

            engine.video->texture_fl = ptr;
            engine.video->texture_flcapacity = new_capacity;
        }

        target = (int)engine.video->texture_flcount;
        engine.video->texture_flcount++;
    }

    sre_defer(deferred_texdestroy, (char*)engine.video->textures + (id - 1) * engine.video->texture_size);
}

SDL_PixelFormatEnum sre_tex_format(sre_Texture id)
{
    void* texture = sre_get_texture(id);
    if (!texture) return SDL_PIXELFORMAT_UNKNOWN;

    return engine.video->interface->tex_format(engine.video, texture);
}

bool sre_tex_size(sre_Texture id, int* w, int* h)
{
    void* texture = sre_get_texture(id);
    if (!texture) return false;

    return engine.video->interface->tex_size(engine.video, texture, w, h);
}