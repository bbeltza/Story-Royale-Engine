#pragma once

#include <Datatypes/Vector.hpp>
#include <SDL_surface.h>

struct sre_Sampler;

namespace sre
{
	class Image
	{
		SDL_Surface* sdl_surface = NULL;
	public:
		explicit Image(const char* path); // Explicit to avoid passing-in a string in a function that takes an image. Ex: void process(const Image& img); ... process("res://img/image.png"); -> This should not work

		Image() = default;
		Image(int w, int h);
		Image(const Image& other);
		Image(Image&& other) noexcept;

		~Image();

		explicit Image(SDL_Surface* from_surface): sdl_surface(from_surface) {}
		explicit Image(const sre::vec2i& size): Image(size.x, size.y) {}

		void blit(const Image& img, const sre::vec2i& pos, const sre::vec2f& anchor);
		bool valid() const { return sdl_surface != NULL; }
		void* handle() const { return sdl_surface; }

		sre::vec2i size() const { return valid() ? vec2i{ sdl_surface->w, sdl_surface->h } : vec2i::ZERO; }
		SDL_PixelFormatEnum SDLformat() const { return valid() ? static_cast<SDL_PixelFormatEnum>(sdl_surface->format->format) : SDL_PIXELFORMAT_UNKNOWN; }

		sre_Sampler* to_sampler() const;
	};
}