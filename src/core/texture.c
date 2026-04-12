#include <Core/Render.h>
#include <Core/Defer.h>

#include "../internal.h"

#include <utils/mem.h>

sre_Sampler* texture_alloc(sre_RenderInterface* render)
{
    return sre_new(engine.video_tsize);
}

void texture_free(sre_RenderInterface* render, sre_Sampler* sampler)
{
    sre_delete(sampler);
}

struct d_setup_texture
{
    sre_RenderInterface* render;
    sre_Sampler* sampler;
    sre_pixelFormat formathint;
    int w, h;
};
static sre_sptr d_setup_texture(void* _data)
{
    const struct d_setup_texture* data = _data;
    return data->render->vftptr->setup_texture(
        data->render,
        data->sampler,
        data->formathint,
        data->w, data->h
    );
}

struct d_update_texture
{
    sre_RenderInterface* render;
    sre_Sampler* sampler;
    const void* pixels;
    int pitch;
};

static sre_sptr d_update_texture(void* _data)
{
    const struct d_update_texture* data = _data;
    return data->render->vftptr->update_texture(
        data->render,
        data->sampler,
        data->pixels, data->pitch
    );
}

struct d_destroy_texture
{
    sre_RenderInterface* render;
    sre_Sampler* sampler;
};

static void d_destroy_texture(void* _data)
{
    struct d_destroy_texture* data = _data;
    data->render->vftptr->destroy_texture(data->render, data->sampler);
    texture_free(data->render, data->sampler);
    sre_delete(data);
}

//

sre_Sampler* sre_RI_sampler(sre_RenderInterface* render, sre_pixelFormat formathint, int w, int h)
{
    sre_Sampler* sampler = texture_alloc(render); assert(sampler != NULL);
    sre_defer_response(d_setup_texture, &(struct d_setup_texture){ render, sampler, formathint, w, h });
    return sampler;
}

void sre_RI_samplerdestroy(sre_RenderInterface* render, sre_Sampler* sampler)
{
    if (!sampler) return;
    assert(render != NULL);

    struct d_destroy_texture* ddata = sre_new(sizeof(struct d_destroy_texture));
    ddata->render = render;
    ddata->sampler = sampler;

    sre_defer(d_destroy_texture, ddata);
}

bool sre_RI_samplerupdate(sre_RenderInterface* render, sre_Sampler* sampler, const void* pixels, int pitch)
{
    return sre_defer_response(d_update_texture, &(struct d_update_texture){ render, sampler, pixels, pitch });
}

bool sre_RI_samplerquery(sre_RenderInterface* render, sre_Sampler* sampler, int size[2], sre_pixelFormat* format)
{
    return render->vftptr->query_texture(render, sampler, size, format);
}