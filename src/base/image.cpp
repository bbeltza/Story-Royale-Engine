#include <SDL_image.h>
#include <Base/Image.hpp>
#include <Base/File.hpp>
#include <Base/Error.h>

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
        sre::error(SRE_ERR_CREATE, "Failed loading image from file");
        return;
    }

    SDL_RWops* rw = file.to_RWops();
    this->sdl_surface = IMG_Load_RW(rw, 1);

    if (!this->sdl_surface)
    {
        sre::error(SRE_ERR_CREATE, "Failed loading image from IMG_Load_RW");
        sre::error(SRE_ERR_SDL);
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

void Image::operator=(SDL_Surface* from_surface)
{
    if (sdl_surface)
        SDL_FreeSurface(sdl_surface);
    
    sdl_surface = from_surface;
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

sre::Sampler* Image::to_sampler() const
{
    if (!sdl_surface)
        return sre::error(SRE_ERR_CREATE, "Image doesn't contain any surface, it is not valid.") ? nullptr : nullptr;

    sre::Sampler* sampler = sre::sampler(static_cast<sre::pixelFormat>(this->SDLformat()), sdl_surface->w, sdl_surface->h);
    if (!sampler)
        return NULL;

    sre::pixelFormat format = sampler->format();
    if (format == SDLformat())
    {
        if (!sampler->update(sdl_surface->pixels, sdl_surface->pitch))
            return NULL;
    }
    else
    {
        SDL_Surface *copy = SDL_ConvertSurfaceFormat(sdl_surface, format, 0);
        if (!copy)
            return sre::error(SRE_ERR_SDL) ? nullptr : nullptr;
        else
        {
            if (!sampler->update(copy->pixels, copy->pitch))
                return NULL;
            
            SDL_FreeSurface(copy);
        }
    }

    return sampler;
}