#include <Core/Render.h>
#include <Core/Defer.h>
#include <Base/Error.h>

#include "../internal.h"

#include <utils/mem.h>

struct sre_Sampler
{
    int refcount;
    int w, h;
    sre_pixelFormat format;
};

struct _texture_container
{
	struct _texture_container* next;
	char data[1];
};

#define ARENA_TEXTURE_COUNT 64
#define TEXTURE_SIZE (sizeof(sre_Sampler) + engine.video.texture_size)

#define FL_BASE 0
#define FL_SIZE 1
#define FL_CAP 2

void __cleanup_textures()
{
    while (engine.video.textures.head)
    {
        struct _texture_container* current = engine.video.textures.head;
        engine.video.textures.head = current->next;

        for (size_t i = 0; i < engine.video.textures.last; i++)
        {
            bool isinfreelist = false;
            sre_Sampler* sampler = (sre_Sampler*)&current->data[i*TEXTURE_SIZE];
            for (sre_Sampler** fi = engine.video.textures.freelist[FL_BASE]; fi < engine.video.textures.freelist[FL_SIZE]; fi++)
            {
                if (*fi != sampler)
                    continue;
            
                isinfreelist = true;
                break;
            }
            if (isinfreelist) continue;

            SRE_VIDEO(engine.video.vfptr, texture_destroy, sampler+1);
        }

        engine.video.textures.last = ARENA_TEXTURE_COUNT;
        free(current);
    }
}

sre_Sampler* texture_alloc()
{
    if (engine.video.textures.freelist[FL_BASE] != engine.video.textures.freelist[FL_SIZE])
        return *--engine.video.textures.freelist[FL_SIZE];

    if (!engine.video.textures.head || engine.video.textures.last >= ARENA_TEXTURE_COUNT)
    {
        struct _texture_container* lasthead = engine.video.textures.head;
        engine.video.textures.head = malloc(offsetof(struct _texture_container, data) + TEXTURE_SIZE * ARENA_TEXTURE_COUNT);
        if (!engine.video.textures.head)
            return NULL;  
        
        engine.video.textures.head->next = lasthead;
        engine.video.textures.last = 0;
    }

    sre_Sampler* sampler = (sre_Sampler*)&engine.video.textures.head->data[engine.video.textures.last * TEXTURE_SIZE];
    engine.video.textures.last++;
    return sampler;
}

void texture_free(sre_Sampler* sampler)
{
    if (engine.video.textures.freelist[FL_SIZE] >= engine.video.textures.freelist[FL_CAP])
    {
        size_t last_size = engine.video.textures.freelist[FL_SIZE] - engine.video.textures.freelist[FL_BASE];
        size_t last_cap = engine.video.textures.freelist[FL_CAP] - engine.video.textures.freelist[FL_BASE];
        size_t newcap = last_cap ? last_cap*2 : 32;
        engine.video.textures.freelist[FL_BASE] = realloc(engine.video.textures.freelist[FL_BASE], newcap*sizeof(void*));
        assert(engine.video.textures.freelist != NULL);

        engine.video.textures.freelist[FL_SIZE] = engine.video.textures.freelist[FL_BASE] + last_size;
        engine.video.textures.freelist[FL_CAP] = engine.video.textures.freelist[FL_BASE] + newcap;
    }

    *engine.video.textures.freelist[FL_SIZE] = sampler;
    engine.video.textures.freelist[FL_SIZE]++;
}

struct d_setup_texture
{
    sre_Sampler* sampler;
    sre_pixelFormat formathint;
    int w, h;
    sre_pixelFormat* outformat;
};
static sre_sptr d_setup_texture(void* _data)
{
    const struct d_setup_texture* data = _data;
    return SRE_VIDEO(engine.video.vfptr,
        texture_setup,
        data->sampler+1,
        data->formathint,
        data->w, data->h,
        data->outformat
    );
}

struct d_update_texture
{
    sre_Sampler* sampler;
    const sre_rect2Di* region;
    const void* pixels;
    int pitch;
};

static sre_sptr d_update_texture(void* _data)
{
    const struct d_update_texture* data = _data;
    return SRE_VIDEO(engine.video.vfptr,
        texture_update,
        data->sampler+1,
        data->region, data->pixels, data->pitch
    );
}

struct d_destroy_texture
{
    sre_Sampler* sampler;
};

static void d_destroy_texture(void* _data)
{
    struct d_destroy_texture* data = _data;
    SRE_VIDEO(engine.video.vfptr, texture_destroy, data->sampler+1);
    texture_free(data->sampler);
    sre_delete(data);
}

//

sre_Sampler* sre_sampler(sre_pixelFormat formathint, int w, int h)
{
    sre_Sampler* sampler = texture_alloc(); assert(sampler != NULL);
    if (!sre_defer_response(d_setup_texture, &(struct d_setup_texture){ sampler, formathint, w, h, &sampler->format }))
    {
        texture_free(sampler);
        return false;
    }

    sampler->refcount = 1;
    sampler->w = w;
    sampler->h = h;
    return sampler;
}

bool sre_sampler_update(sre_Sampler* sampler, const sre_rect2Di* region, const void* pixels, int pitch)
{
    if (!sampler)
        return sre_error(SRE_ERR_INVALID_PARAMETER, "Parameter `sampler` is NULL") && false;
    if (!pixels)
        return sre_error(SRE_ERR_INVALID_PARAMETER, "Parameter `pixels` is NULL") && false;

    sre_rect2Di outregion;
    if (!region)
    {
        outregion = (sre_rect2Di){
            .position = { 0, 0 },
            .size = { sampler->w, sampler->h }
        };
    }
    else
    {
        outregion = *region;
        if (outregion.x < 0 || outregion.y < 0)
            return sre_error(SRE_ERR_INVALID_VALUE, "`region`'s coordinates are negative. This is not possible since base coordinates start at the top-left corner of the texture, so writing to negative coordinates would overflow") && false;
        
        if (outregion.w <= 0 || outregion.h <= 0)
            return sre_error(SRE_ERR_INVALID_VALUE, "`region`'s size is lower or equal to zero. You won't be able to write to a texture") && false;

        if ((outregion.x + outregion.w) > sampler->w || (outregion.y + outregion.h) > sampler->h)
            return sre_error(SRE_ERR_INVALID_VALUE, "`region` overflows the total texture area.") && false;
    }

    return sre_defer_response(d_update_texture, &(struct d_update_texture){ sampler, &outregion, pixels, pitch });
}

bool sre_sampler_query(sre_Sampler* sampler, int size[2], sre_pixelFormat* format)
{
    if (!sampler) return false;

    if (size)
    {
        size[0] = sampler->w;
        size[1] = sampler->h;
    }
    if (format)
    {
        *format = sampler->format;
    }

    return true;
}

int sre_sampler_aquire(sre_Sampler* sampler)
{
    return SDL_AtomicAdd((SDL_atomic_t*)&sampler->refcount, 1);
}

int sre_sampler_release(sre_Sampler* sampler)
{
    if (!sampler) return 0;
    if (!engine.video.vfptr) return 0;

    int ref = SDL_AtomicAdd((SDL_atomic_t*)&sampler->refcount, -1);
    if (ref != 1)
        return ref;
    
    struct d_destroy_texture* ddata = sre_new(sizeof(struct d_destroy_texture));
    ddata->sampler = sampler;

    sre_defer(d_destroy_texture, ddata);
    return ref;
}