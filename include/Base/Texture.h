#ifndef SRE_TEXTURE_H
#define SRE_TEXTURE_H
#include <C_API.h>
#include <ints.h>

#include <SDL_surface.h>

SRE_CAPI_BEGIN

// Texture identifier
// A texture is an image managed by the video driver to be rendered
typedef sre_u32 sre_Texture;

sre_Texture sre_tex_gen();
int sre_tex_bind(sre_Texture id, const SDL_Surface* surface);
int sre_tex_update(sre_Texture id, const void* pixels, int pitch);
int sre_tex_size(sre_Texture id, int* w, int* h);
SDL_PixelFormatEnum sre_tex_format(sre_Texture id);
void sre_tex_destroy(sre_Texture id);

// Get driver-speficic pointer to a texture id
// It is only meant to be used by driver implementations and the sre_tex_ implementation
// @param id The texture id
// @return The pointer to the pointer to the texture, or NULL if it's unavailable
void* sre_get_texture(sre_Texture id);

SRE_CAPI_END

#endif