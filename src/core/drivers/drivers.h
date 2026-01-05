#ifndef SRE_DRIVERS_H
#define SRE_DRIVERS_H
#include <SDL.h>

#include <Base/Draw.h>
#include <datatypes/units.h>

#define SRE_DRAW_FUNC(name, signature) int (*name)(signature)

typedef struct sre_videodriver
{
	int (*vsync)(int vsync);
	int (*quit)();
	int (*viewport)(int w, int h);

	void (*present)();

	SRE_DRAW_FUNC(draw_fill, const sre_DDFill* data);
	SRE_DRAW_FUNC(draw_line, const sre_DDLine* data);
	SRE_DRAW_FUNC(draw_lines, const sre_DDLines* data);

	sre_unit camera_x;
	sre_unit camera_y;
	sre_unit scale;
} sre_videodriver;

#endif