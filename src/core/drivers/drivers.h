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
#define SRE_DRAW_FUNC(name, signature) int (*name)(signature)

typedef struct sre_videodriver
{
	int (*vsync)(int vsync);
	int (*quit)();
	int (*viewport)(int w, int h);

	void (*present)();

	const char* error;

	SRE_DRAW_FUNC(draw_clear, const sre_u8 col[4]);
	
	SRE_DRAW_FUNC(draw_fill, const sre_DDFill* data);
	SRE_DRAW_FUNC(draw_line, const sre_DDLine* data);
	SRE_DRAW_FUNC(draw_lines, const sre_DDLines* data);
	SRE_DRAW_FUNC(draw_rect, const sre_DDRect* data);
	SRE_DRAW_FUNC(draw_rrect, const sre_DDRRect* data);
	SRE_DRAW_FUNC(draw_texture, const sre_DDTexture* data);
	SRE_DRAW_FUNC(draw_rtexture, const sre_DDRTexture* data);

	sre_unit camera_x;
	sre_unit camera_y;
	sre_unit center_x;
	sre_unit center_y;
	sre_unit scale;
} sre_videodriver;

#endif