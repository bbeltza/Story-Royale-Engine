#include <SDL_image.h>
#include <Base/Image.hpp>
#include <Base/File.hpp>

using namespace sre;

Image::Image(unsigned w, unsigned h, int depth):
    sdl_surface(SDL_CreateRGBSurfaceWithFormat(NULL, w, h, depth, SDL_PIXELFORMAT_RGBA8888))
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
    }
}

Image::~Image()
{
    SDL_Surface* tmp = sdl_surface;
    sdl_surface = NULL;

    SDL_FreeSurface(tmp);
}