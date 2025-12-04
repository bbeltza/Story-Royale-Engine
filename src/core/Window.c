#include "../internal.h"
#include "GameSettings.h"

#define CENTERED SDL_WINDOWPOS_CENTERED

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
		CENTERED,
		CENTERED,
		w,
		h,
		flags
	);
    SDL_SetWindowMinimumSize(engine.sdl_windowhndl, 200, 200);
}

int __poll_events()
{
    while (SDL_PollEvent(&engine.sdl_eventhndl) != 0)
    {
        switch (engine.sdl_eventhndl.type)
        {
        case SDL_WINDOWEVENT:
            switch (engine.sdl_eventhndl.window.event)
            {
            case SDL_WINDOWEVENT_FOCUS_LOST:
                SDL_ResetKeyboard();
                break;
            }
            break;
        case SDL_USEREVENT:
            switch (engine.sdl_eventhndl.user.code)
            {
            case 1:
                SDL_ShowWindow(engine.sdl_windowhndl);
                SDL_RaiseWindow(engine.sdl_windowhndl);
                break;
            }
            break;
        case SDL_QUIT:
            return 0;
        default:
            __poll_input();
            break;
        }
    }

    return 1;
}