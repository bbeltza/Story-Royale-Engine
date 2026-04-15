#include "d3d11.h"

using namespace sreD3D11;

bool Instance::setup_texture(sre::Sampler* texture, sre::pixelFormat format, int x, int y)
{
	texture->w = x;
	texture->h = y;
	return true;
}
bool Instance::update_texture(sre::Sampler* texture, const void* pixels, int pitch = 0)
{
	return false;
}

bool Instance::query_texture(sre::Sampler* texture, sre::vec2i* size, sre::pixelFormat* format)
{
	if (size)
		*size = { texture->w, texture->h };
	if (format)
		*format = SDL_PIXELFORMAT_ABGR8888;

	return true;
}

void Instance::destroy_texture(sre::Sampler* texture) {}