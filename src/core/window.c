#include "../internal.h"
#include <GameSettings.h>

void __create_window()
{
    uint32_t flags = 0;
    int w, h;

    w = game_settings.WindowOptions.Resolution[0] * (1 + game_settings.WindowOptions.Scaled);
    h = game_settings.WindowOptions.Resolution[1] * (1 + game_settings.WindowOptions.Scaled);

    //if (game_settings.WindowOptions.Hidden)
        flags |= SDL_WINDOW_HIDDEN;
    if (game_settings.WindowOptions.Resizable)
        flags |= SDL_WINDOW_RESIZABLE;

        
	engine.sdl_windowhndl = SDL_CreateWindow(
		game_settings.Title,
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		w,
		h,
		flags
	);

    SDL_SetWindowMinimumSize(engine.sdl_windowhndl, 200, 200);
    engine.osize_x = w;
    engine.osize_y = h;
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