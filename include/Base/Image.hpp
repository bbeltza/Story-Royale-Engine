#pragma once

#include <SDL_surface.h>

namespace sre
{
	class Image
	{
		SDL_Surface* sdl_surface = NULL;
	public:
		Image() = default;
		Image(unsigned w, unsigned h);
		Image(const char* path);
		Image(const Image& other);
		Image(Image&& other);

		~Image();

		Image(SDL_Surface* from_surface): sdl_surface(from_surface) {}

		//void blit(const Image& img);
		bool isValid() const { return sdl_surface != NULL; }
		void* getHandle() const { return sdl_surface; }

	public:
		static SDL_Surface* copySDLsurface(const SDL_Surface* other);
	};
}