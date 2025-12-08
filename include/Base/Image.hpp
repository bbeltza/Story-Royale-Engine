#pragma once

#include <SDL_surface.h>

namespace sre
{
	class Image
	{
		SDL_Surface* sdl_surface = NULL;
	public:
		Image(unsigned w, unsigned h);
		Image(const char* path);

		//void blit(const Image& img);
	};
}