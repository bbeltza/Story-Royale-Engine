#ifndef SRE_DRIVERS_H
#define SRE_DRIVERS_H
#include <SDL.h>

#include <Core/Draw.h>
#include <Core/Texture.h>
#include <Datatypes/Units.h>

// Function signature macro for declaring drawing command function pointers.
// Used by `sre_videodriver` for every drawing command
// @param name the name of the function, in `sre_videodriver`, it's one of the `draw_xx` functions
// @param signature the parameter to take in, these functions do only take a single argument, there might be an additional parameter for the video driver pointer too
//
// @return the value to return in `sre_draw()`: Usually 0 on success or -1 on error
#define SRE_DRAW_FUNC(name, signature) bool (*name)(const sre_videodriver* video, signature)

#define SRE_TEXTURE_BASECOUNT 24

typedef struct sre_videodriver sre_videodriver;

typedef bool (*sre_videoinit_func)(sre_videodriver* video, SDL_Window* window);
struct sre_videodriverInterface
{
	void (*quit)(sre_videodriver* video);

	void (*present)(const sre_videodriver* video);

	bool (*viewport)(const sre_videodriver* video, int w, int h); // Resize the buffer, optional
	bool (*vsync)(const sre_videodriver* video, int vsync);
	bool (*blend)(const sre_videodriver* video, sre_DrawBlending blending);

	bool (*tex_gen)(const sre_videodriver* video, void* texture);
	bool (*tex_update)(const sre_videodriver* video, void* texture, const void* pixels, int pitch);
	bool (*tex_bind)(const sre_videodriver* video, void* texture, const SDL_Surface* surface);
	bool (*tex_size)(const sre_videodriver* video, void* texture, int* w, int* h);
	void (*tex_destroy)(const sre_videodriver* video, void* texture);
	SDL_PixelFormatEnum (*tex_format)(const sre_videodriver* video, void* texture);

	SRE_DRAW_FUNC(draw_clear, const sre_col4* color);
	SRE_DRAW_FUNC(draw_clip, const sre_rect2Dut* rect);
	
	SRE_DRAW_FUNC(draw_fill, const sre_DDFill* data);
	SRE_DRAW_FUNC(draw_line, const sre_DDLine* data);
	SRE_DRAW_FUNC(draw_lines, const sre_DDLines* data);
	SRE_DRAW_FUNC(draw_rect, const sre_DDRect* data);
	SRE_DRAW_FUNC(draw_rrect, const sre_DDRRect* data);
	SRE_DRAW_FUNC(draw_texture, const sre_DDTexture* data);
	SRE_DRAW_FUNC(draw_rtexture, const sre_DDRTexture* data);
};

#ifndef IMGUI_DISABLE
	struct ImDrawData;
	struct sre_videodriverImGuiInterface
	{
		bool (*imgui_init)(const sre_videodriver* video);
		void (*imgui_newframe)();
		void (*imgui_renderdrawdata)(struct ImDrawData* ImDrawData, const sre_videodriver* video);
	};
#endif

struct sre_videodriver
{
	struct sre_videodriverInterface const* interface;
	#ifndef IMGUI_DISABLE
		struct sre_videodriverImGuiInterface const* imgui;
	#endif

	/*
		void (*quit)(sre_videodriver* video);
		void (*present)(const sre_videodriver* video);

		bool (*vsync)(const sre_videodriver* video, int vsync);
		bool (*viewport)(const sre_videodriver* video, int w, int h);

		bool (*blend)(const sre_videodriver* video, sre_DrawBlending blending);

		SRE_DRAW_FUNC(draw_clear, const sre_col4* color);
		SRE_DRAW_FUNC(draw_clip, const sre_rect2Dut* rect);

		SRE_DRAW_FUNC(draw_fill, const sre_DDFill* data);
		SRE_DRAW_FUNC(draw_line, const sre_DDLine* data);
		SRE_DRAW_FUNC(draw_lines, const sre_DDLines* data);
		SRE_DRAW_FUNC(draw_rect, const sre_DDRect* data);
		SRE_DRAW_FUNC(draw_rrect, const sre_DDRRect* data);
		SRE_DRAW_FUNC(draw_texture, const sre_DDTexture* data);
		SRE_DRAW_FUNC(draw_rtexture, const sre_DDRTexture* data);

		bool (*tex_gen)(const sre_videodriver* video, void* texture);
		bool (*tex_update)(const sre_videodriver* video, void* texture, const void* pixels, int pitch);
		bool (*tex_bind)(const sre_videodriver* video, void* texture, const SDL_Surface* surface);
		bool (*tex_size)(const sre_videodriver* video, void* texture, int* w, int* h);
		SDL_PixelFormatEnum (*tex_format)(const sre_videodriver* video, void* texture);
		void (*tex_destroy)(const sre_videodriver* video, void* texture);
	*/

	char* error; // Error buffer/pointer, WIP
	void* userdata;

	void* textures;
	/* These are read-only members, they aren't meant to be modified by the driver */
	/* They are used by the engine to send the correct texture pointer to the driver, based on a texture id */
	size_t texture_size; // Size in bytes of a driver-speficic texture
	size_t textures_count;
	size_t textures_capacity;
	sre_Texture* texture_fl; // Texture free list
	size_t texture_flcount;
	size_t texture_flcapacity;

	sre_rect2Dut* clipstack_base;
	size_t clipstack_pos;
	size_t clipstack_size;

	sre_vec2ut size;
	sre_vec2ut center;
	sre_vec2ut camera;
	sre_unit scale;
};

#endif