#include <Core/Window.h>
#include "../internal.h"

extern const char __game_title[];

void __create_window()
{	
    static const int DEFAULT_WIDTH = 640;
    static const int DEFAULT_HEIGHT = 480;
    
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 5);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);

	// TODO: Should give the render driver more control on how to create the window and setup its flags!
	engine.sdl_windowhndl = SDL_CreateWindow(
		__game_title,
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		DEFAULT_WIDTH,
		DEFAULT_HEIGHT,
		SDL_WINDOW_HIDDEN | SDL_WINDOW_OPENGL // D3D12's debug layer breaks when having the OpenGL flag, I don't know what to do now since you need that flag to use OpenGL. Maybe let the render driver chose extra flags?
	);
}

sre_Window* sre_mainwindow(void) { return (sre_Window*)(engine.sdl_windowhndl); }