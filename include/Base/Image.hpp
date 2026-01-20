#pragma once

#include <Datatypes/Vector.hpp>
#include <SDL_surface.h>

namespace sre
{
	class Image
	{
		SDL_Surface* sdl_surface = NULL;
	public:
		explicit Image(const char* path); // Explicit to avoid passing-in a string in a function that takes an image

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

	public:
		static SDL_Surface* copy_SDLsurface(const SDL_Surface* other);
	};
}