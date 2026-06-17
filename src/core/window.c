#include <Core/Window.h>
#include <Hints.h>

#include "../internal.h"

extern const char __game_title[];

void __create_window()
{	
    static const int DEFAULT_WIDTH = 640;
    static const int DEFAULT_HEIGHT = 480;
    
	static const int DEFAULT_FLAGS = SDL_WINDOW_HIDDEN | SDL_WINDOW_OPENGL;
	const int* flagshint = sre_gethint("SDL_WINDOWFLAGS");
	int flags = DEFAULT_FLAGS;
	if (flagshint)
	{
		flags = *flagshint;
		if (!(flags & SDL_WINDOW_SHOWN))
			flags |= SDL_WINDOW_HIDDEN;
		else {
			assert((flags & SDL_WINDOW_SHOWN) && (flags & SDL_WINDOW_HIDDEN));
		}
	}

	if (flags & SDL_WINDOW_OPENGL)
	{
		SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
		SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 5);
		SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);
	}

	// TODO: Should give the render driver more control on how to create the window and setup its flags!
	engine.sdl_windowhndl = SDL_CreateWindow(
		__game_title,
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		DEFAULT_WIDTH,
		DEFAULT_HEIGHT,
		flags
	);
}

sre_Window* sre_mainwindow(void) { return (sre_Window*)(engine.sdl_windowhndl); }
bool sre_defer_windowcommand(sre_Window* window, enum sreWindowCmd cmd,  ...)
{
	if (!window)
		return false; // Please, call sre_mainwindow() if you refer to the main window

	switch (cmd)
	{
		case SRE_WINDOWCMD_SETPOS:
		case SRE_WINDOWCMD_SETSIZE:
		case SRE_WINDOWCMD_SETFULLSCREEN:
		case SRE_WINDOWCMD_SETTITLE:
		case SRE_WINDOWCMD_MINIMIZE:
		case SRE_WINDOWCMD_MAXIMIZE:
		case SRE_WINDOWCMD_HIDE:
		case SRE_WINDOWCMD_SHOW:
		case SRE_WINDOWCMD_FLASH:
			break;

		default:
			// sre_error(SRE_ERR_INVALID_ENUM, cmd)
			return false;
	}

	return true;
}