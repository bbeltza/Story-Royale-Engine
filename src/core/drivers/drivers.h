#ifndef SRE_DRIVERS_H
#define SRE_DRIVERS_H
#include <SDL.h>

typedef struct
{
	int (*init)(SDL_Window* window);
	int (*quit)();
	int (*viewport)(int w, int h);

	int (*present)();
} sre_videodriver;

#endif