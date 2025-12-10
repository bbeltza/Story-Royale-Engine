#pragma once

#include <SDL_surface.h>

namespace sre
{
	class Image
	{
		SDL_Surface* sdl_surface = NULL;
	public:
		Image(unsigned w, unsigned h, int depth=32);
		Image(const char* path);

		~Image();

		//void blit(const Image& img);
		bool isValid() const { return sdl_surface != NULL; }
	};
}