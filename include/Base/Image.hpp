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
		Image(Image&& other);

		~Image();

		Image(SDL_Surface* from_surface): sdl_surface(from_surface) {}

		void blit(const Image& img, const Vector2i& pos, const Vector2f& anchor);
		bool isValid() const { return sdl_surface != NULL; }
		void* getHandle() const { return sdl_surface; }

	public:
		static SDL_Surface* copySDLsurface(const SDL_Surface* other);
	};
}