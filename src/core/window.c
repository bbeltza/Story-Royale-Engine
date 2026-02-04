#include "../internal.h"

extern const char __game_title[];

void __create_window()
{
    static const DEFAULT_WIDTH = 640;
    static const DEFAULT_HEIGHT = 480;
        
	engine.sdl_windowhndl = SDL_CreateWindow(
		__game_title,
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		DEFAULT_WIDTH,
		DEFAULT_HEIGHT,
		SDL_WINDOW_HIDDEN | SDL_WINDOW_RESIZABLE
	);

    SDL_SetWindowMinimumSize(engine.sdl_windowhndl, 200, 200);
    engine.osize_x = DEFAULT_WIDTH;
    engine.osize_y = DEFAULT_HEIGHT;
}

int __poll_events()
{
    engine.frame++;

    SDL_Event ev;

    while (SDL_PollEvent(&ev) != 0)
    {
        switch (ev.type)
        {
        case SDL_WINDOWEVENT:
            switch (ev.window.event)
            {
            case SDL_WINDOWEVENT_FOCUS_LOST:
                SDL_ResetKeyboard();
                break;
            }
            break;
        case SDL_USEREVENT:
            switch (ev.user.code)
            {
            case 1:
                SDL_ShowWindow(engine.sdl_windowhndl);
                SDL_RaiseWindow(engine.sdl_windowhndl);
                __update_viewport(engine.osize_x, engine.osize_y);
                break;
            }
            break;
        case SDL_QUIT:
            return 0;
        default:
            __poll_input(&ev);
            break;
        }
    }

    return 1;
}