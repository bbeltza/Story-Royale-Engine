#include <Core/Render.h>
#include <Core/Defer.h>

#include "../internal.h"

#include <utils/mem.h>

struct sre_Sampler
{
    int refcount;
    int w, h;
    sre_pixelFormat format;
};

#include <Base/Log.h>

sre_Sampler* texture_alloc()
{
    return sre_new(sizeof(sre_Sampler) + engine.video.texture_size);
}

void texture_free(sre_Sampler* sampler)
{
    free(sampler);
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
    const void* pixels;
    int pitch;
};

static sre_sptr d_update_texture(void* _data)
{
    const struct d_update_texture* data = _data;
    return SRE_VIDEO(engine.video.vfptr,
        texture_update,
        data->sampler+1,
        data->pixels, data->pitch
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

bool sre_sampler_update(sre_Sampler* sampler, const void* pixels, int pitch)
{
    return sre_defer_response(d_update_texture, &(struct d_update_texture){ sampler, pixels, pitch });
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
    assert(engine.video.vfptr != NULL);

    int ref = SDL_AtomicAdd((SDL_atomic_t*)&sampler->refcount, -1);
    if (ref != 1)
        return ref;
    
    struct d_destroy_texture* ddata = sre_new(sizeof(struct d_destroy_texture));
    ddata->sampler = sampler;

    sre_defer(d_destroy_texture, ddata);
    return ref;
}