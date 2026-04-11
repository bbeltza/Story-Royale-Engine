#include <SDL_image.h>
#include <Base/Image.hpp>
#include <Base/File.hpp>

#include <Core/Render.h>

using namespace sre;

Image::Image(int w, int h):
    sdl_surface(SDL_CreateRGBSurfaceWithFormat(0, w, h, 0, SDL_PIXELFORMAT_RGBA8888))
{
}

Image::Image(const char* path)
{
    File file(path, FILE_READ);
    if (!file.valid())
    {
        sre::log<LOGCATEGORY_ERROR>("Image::Image: Failed loading %s");
        return;
    }

    SDL_RWops* rw = file.to_RWops();
    this->sdl_surface = IMG_Load_RW(rw, 1);

    if (!this->sdl_surface)
    {
        sre::log<LOGCATEGORY_ERROR>("Image::Image: Could not load %s, file is not an image or format is unsupported (SDL Error: `%s`)", path, IMG_GetError());
        return;
    }
}

Image::Image(const Image& other): sdl_surface(SDL_DuplicateSurface(other.sdl_surface))
{
}

Image::Image(Image&& other) noexcept : sdl_surface(other.sdl_surface)
{
    other.sdl_surface = NULL;
}

Image::~Image()
{
    SDL_Surface* tmp = sdl_surface;
    sdl_surface = NULL;

    SDL_FreeSurface(tmp);
}

void Image::blit(const Image& img, const sre::vec2i& pos, const sre::vec2f& anchor)
{
    SDL_Rect rect = {
        pos.x,
        pos.y,
        0,
        0
    };
    if (anchor.x)
        rect.x -= static_cast<int>(img.sdl_surface->w * anchor.x);
    if (anchor.y)
        rect.y -= static_cast<int>(img.sdl_surface->h * anchor.y);


    SDL_UpperBlit(img.sdl_surface, NULL, sdl_surface, &rect);
}

sre_Sampler* Image::to_sampler() const
{
    #define IMG_TSERR(...) sre::log<LOGCATEGORY_ERROR>("Image::to_sampler() - " __VA_ARGS__)

    if (!sdl_surface)
    {
        IMG_TSERR("image does not contain any surface. It is not valid");
        return NULL;
    }

    sre::RenderInterface* renderer = sre::get_renderer();

    sre_Sampler* sampler = renderer->sampler(static_cast<sre::pixelFormat>(this->SDLformat()), sdl_surface->w, sdl_surface->h);
    if (!sampler)
    {
        IMG_TSERR("RenderInterface::sampler() failed");
        return NULL;
    }

    sre::pixelFormat format;
    renderer->sampler_query(sampler, NULL, &format);

    if (format == SDLformat())
    {
        if (!renderer->sampler_update(sampler, sdl_surface->pixels, sdl_surface->pitch))
            IMG_TSERR("RenderInterface::sampler_update() failed");
    }
    else
    {
        SDL_Surface *copy = SDL_ConvertSurfaceFormat(sdl_surface, format, 0);
        if (!copy)
            IMG_TSERR("SDL_ConvertSurfaceFormat() failed: %s", SDL_GetError());
        else
        {
            if (!renderer->sampler_update(sampler, copy->pixels, copy->pitch))
                IMG_TSERR("RenderInterface::sampler_update() failed");
            
            SDL_FreeSurface(copy);
        }
    }

    return sampler;
}