#include "swrender.h"

#include <SDL_timer.h>
#include <assert.h>

#include <utils/math.h>

static void sresw_destroy(void* _inst);
static const struct sre_RenderVFT sresw_vft = {
    .destructor = sresw_destroy,
    .draw1 = sresw_draw1,
    .draw2 = sresw_draw2,
    .begin = sresw_begin,
    .end = sresw_end,
    .set_viewportstate = sresw_set_viewportstate,
    .set_vsync = sresw_set_vsync,
    .set_texturestate = sresw_set_texturestate,
    .set_blendstate = sresw_set_blendstate,
    .set_camerastate = sresw_set_camerastate,
    .set_scissorstate = sresw_set_scissorstate,
    .texture_setup = sresw_texture_setup,
    .texture_update = sresw_texture_update,
    .texture_destroy = sresw_texture_destroy
};
static int sresw_main(const struct sre_RenderVFT** interface, void* _inst, SDL_Window* window)
{
    sresw_Instance* inst = _inst;
    if (!SDL_GetWindowSurface(window))
        return SRE_RENDERSTATUS_FAILED;
    
    inst->window = window;
    *interface = &sresw_vft;
    return SRE_RENDERSTATUS_SUCCEEDED;
}

static void sresw_destroy(void* _inst)
{
    sresw_Instance* inst = _inst;
    (void)inst;
}

const sre_RenderDriverData sreswrender = {sresw_main, sizeof(sresw_Instance), sizeof(sresw_Texture), "Sowftare"};

//

void sresw_draw1(void* _inst, const sre_RenderInstance1* instances, size_t instance_count)
{
    sresw_Instance* inst = _inst;
    SDL_Surface* wndsurface = inst->wndsurface;

    const int scale = inst->state.scale;
    const int width = inst->state.width;
    const int height = inst->state.height;

    for (size_t i = 0; i < instance_count; i++)
    {
        const sre_RenderInstance1* dinst = &instances[i];
        if (!dinst->color.a)
            continue;

        sre_rect2Di rectangle = {
            (int)(dinst->rectangle.x * scale),
            (int)(dinst->rectangle.y * scale),
            (int)(dinst->rectangle.w * scale),
            (int)(dinst->rectangle.h * scale)
        };
        rectangle.size.x = SDL_abs(rectangle.size.x);
        rectangle.size.y = SDL_abs(rectangle.size.y);

        rectangle.position.x -= (int)(rectangle.w * dinst->anchor.x);
        rectangle.position.y -= (int)(rectangle.h * dinst->anchor.y);

        rectangle.position.x += inst->state.camerax;
        rectangle.position.y += inst->state.cameray;

        int startx, endx;
        int starty, endy;

        startx = ut_max(0, rectangle.x);
        starty = ut_max(0, rectangle.y);

        endx = ut_min(width, rectangle.x + rectangle.w);
        endy = ut_min(height, rectangle.y + rectangle.h);

        Uint32 mappedcolor = SDL_MapRGB(wndsurface->format, dinst->color.r, dinst->color.g, dinst->color.b);

        if (startx >= endx || starty >= endy)
            continue;
        
        for (int y = starty, ptry = starty * wndsurface->pitch + startx * wndsurface->format->BytesPerPixel; y < endy; y++, ptry += wndsurface->pitch)
        {
            for (int x = startx, ptrx = ptry; x < endx; x++, ptrx += wndsurface->format->BytesPerPixel)
            {
                void* pixel = ((Uint8*)wndsurface->pixels + ptrx);
                *(Uint32*)pixel = mappedcolor;
            }
        }
    }
}

void sresw_draw2(void* _inst, const sre_RenderPoint* points, size_t point_count, sre_draw2primitive mode)
{

}

void sresw_begin(void* _inst, const float clear[4])
{
    sresw_Instance* inst = _inst;
    SDL_Surface* wndsurface = inst->wndsurface;
    assert(wndsurface != NULL);
    
    Uint8 r = (Uint8)(clear[0]*255);
    Uint8 g = (Uint8)(clear[1]*255);
    Uint8 b = (Uint8)(clear[2]*255);
    Uint32 mappeduint = SDL_MapRGB(wndsurface->format, r, g, b);

    Uint8* ptr = wndsurface->pixels;
    int bytesize = wndsurface->pitch * wndsurface->h;
    for (int i = 0; i < bytesize; i += wndsurface->format->BytesPerPixel, ptr += wndsurface->format->BytesPerPixel)
        *(Uint32*)ptr = mappeduint; 
}

void sresw_end(void* _inst)
{
    sresw_Instance* inst = _inst;
    SDL_Surface* wndgetsurf = SDL_GetWindowSurface(inst->window);
    if (wndgetsurf != inst->wndsurface)
    {
        inst->wndsurface = wndgetsurf;
        return;
    }

    if (SDL_UpdateWindowSurface(inst->window))
        assert(0 && "SDL_UpdateWindowSurface failed!");

    if (inst->state.waitforrefresh)
    {
        SDL_DisplayMode displaymode;
        SDL_GetWindowDisplayMode(inst->window, &displaymode);

        SDL_Delay(1000/displaymode.refresh_rate);
    }
}    

void sresw_set_viewportstate(void* _inst, int w, int h, sre_unit scale)
{
    sresw_Instance* inst = _inst;
    inst->wndsurface = SDL_GetWindowSurface(inst->window);

    inst->state.width = w;
    inst->state.height = h;
    inst->state.scale = (int)scale;
}

void sresw_set_blendstate(void* _inst, sre_blendMode blendmode)
{
    // WIP
}

void sresw_set_camerastate(void* _inst, sre_unit x, sre_unit y)
{
    sresw_Instance* inst = _inst;
    inst->state.camerax = (int)x;
    inst->state.cameray = (int)y;
}

void sresw_set_scissorstate(void* _inst, const sre_rect2Di* rectangle)
{
    // WIP
}

void sresw_set_texturestate(void* _inst, void* _texture)
{
    // WIP
}

void sresw_set_vsync(void* _inst, bool enable)
{
    sresw_Instance* inst = _inst;
    inst->state.waitforrefresh = (enable != false);
}

bool sresw_texture_setup(void* _inst, void* _texture, sre_SDLpixelFormat formathint, int w, int h, sre_SDLpixelFormat* outformat)
{
    sresw_Instance* inst = _inst;
    *outformat = (sre_SDLpixelFormat)SDL_GetWindowPixelFormat(inst->window);
    return true;
}

bool sresw_texture_update(void* _inst, void* _texture, const sre_rect2Di* region, const void* pixels, int pitch)
{
    return true;
}

void sresw_texture_destroy(void* _inst, void* _texture)
{

}
