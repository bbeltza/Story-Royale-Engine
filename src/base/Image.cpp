#include <SDL_image.h>
#include <Base/Image.hpp>
#include <Base/File.hpp>

using namespace sre;

Image::Image(int w, int h):
    sdl_surface(SDL_CreateRGBSurfaceWithFormat(0, w, h, 0, SDL_PIXELFORMAT_RGBA8888))
{
}

Image::Image(const char* path)
{
    File file(path, "rb");
    if (!file.isValid())
    {
        ERROR("Image::Image: Could not load image %s, no such file or directory", path);
        return;
    }

    SDL_RWops* rw = file.toRWops();
    this->sdl_surface = IMG_Load_RW(rw, 1);

    if (!this->sdl_surface)
    {
        ERROR("Image::Image: Could not load %s, file is not an image or format is unsupported", path);
        return;
    }
}

Image::Image(const Image& other): sdl_surface(copySDLsurface(other.sdl_surface))
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

SDL_Surface* Image::copySDLsurface(const SDL_Surface* other)
{
    SDL_Surface* surface = SDL_CreateRGBSurfaceWithFormat(0, other->w, other->h, 0, other->format->format);
    if (surface)
        SDL_memcpy(surface->pixels, other->pixels, surface->h * surface->pitch);

    return surface;
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