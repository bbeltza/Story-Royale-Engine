#ifndef SRE_DRIVERS_H
#define SRE_DRIVERS_H
#include <SDL.h>

#include <Base/Draw.h>
#include <datatypes/units.h>

// Function signature macro for declaring drawing command function pointers
// Used by `sre_videodriver` for every drawing command
// @param name the name of the function, in `sre_videodriver`, it's one of the `draw_xx` functions
// @param signature the parameter to take in, these functions do only take a single argument, there might be an additional parameter for the video driver pointer too
//
// @return the value to return in `sre_draw()`: Usually 0 on success or -1 on error
#define SRE_DRAW_FUNC(name, signature) int (*name)(const sre_videodriver* video, signature)

#define SRE_TEXTURE_BASECOUNT 24

typedef struct sre_videodriver sre_videodriver;

typedef int (*sre_videoinit_func)(sre_videodriver* video, SDL_Window* window);

typedef struct sre_videodriver
{
	void (*quit)(sre_videodriver* video);
	void (*present)(const sre_videodriver* video);

	int (*vsync)(const sre_videodriver* video, int vsync);
	int (*viewport)(const sre_videodriver* video, int w, int h);


	SRE_DRAW_FUNC(draw_clear, const sre_col4* color);
	
	SRE_DRAW_FUNC(draw_fill, const sre_DDFill* data);
	SRE_DRAW_FUNC(draw_line, const sre_DDLine* data);
	SRE_DRAW_FUNC(draw_lines, const sre_DDLines* data);
	SRE_DRAW_FUNC(draw_rect, const sre_DDRect* data);
	SRE_DRAW_FUNC(draw_rrect, const sre_DDRRect* data);
	SRE_DRAW_FUNC(draw_texture, const sre_DDTexture* data);
	SRE_DRAW_FUNC(draw_rtexture, const sre_DDRTexture* data);

	int (*tex_gen)(const sre_videodriver* video, void* texture);
	int (*tex_update)(const sre_videodriver* video, void* texture, const void* pixels, int pitch);
	int (*tex_bind)(const sre_videodriver* video, void* texture, int w, int h, SDL_PixelFormatEnum format);
	int (*tex_size)(const sre_videodriver* video, void* texture, int* w, int* h);
	void (*tex_destroy)(const sre_videodriver* video, void* texture);

	const char* error;
	void* userdata;

	void* const textures;
	/* These are 'const' members, they aren't meant to be modified by the driver */
	/* They are used by the engine to send the correct texture pointer to the driver, based on a texture id */
	sre_usize texture_size; // Size in bytes of a driver-speficic texture
	const sre_usize textures_count;
	const sre_usize textures_capacity;
	const sre_u32* const texture_fl; // Texture free list
	const sre_usize texture_flcount;
	const sre_usize texture_flcapacity;

	sre_vec2ut size;
	sre_vec2ut center;
	sre_vec2ut camera;
	sre_unit scale;
} sre_videodriver;

#endif