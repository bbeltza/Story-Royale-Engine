#include "../internal.h"

extern const char __game_title[];

void __create_window()
{
    static const int DEFAULT_WIDTH = 640;
    static const int DEFAULT_HEIGHT = 480;
    
	engine.sdl_windowhndl = SDL_CreateWindow(
		__game_title,
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		DEFAULT_WIDTH,
		DEFAULT_HEIGHT,
		SDL_WINDOW_HIDDEN
	);

    SDL_SetWindowMinimumSize(engine.sdl_windowhndl, 200, 200);
    engine.osize_x = DEFAULT_WIDTH;
    engine.osize_y = DEFAULT_HEIGHT;
}