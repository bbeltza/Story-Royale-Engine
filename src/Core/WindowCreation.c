#include "../internal.h"

#define CENTERED SDL_WINDOWPOS_CENTERED

#include <SDL_image.h>
#ifdef __unix__
    extern const char _game_ico[];
    extern int _game_icosize;
#endif

void __create_window(const struct _win_settings* _win)
{
	engine.sdl_windowhndl = SDL_CreateWindow(
		_win->title,
		CENTERED,
		CENTERED,
		_win->w,
		_win->h,
		_win->flags
	);
    SDL_SetWindowMinimumSize(engine.sdl_windowhndl, 200, 200);
    #ifdef __unix__
        SDL_SetWindowIcon(engine.sdl_windowhndl, IMG_Load_RW(SDL_RWFromConstMem(_game_ico, _game_icosize), 1));
    #endif
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
        case SDL_QUIT:
            return 0;
        default:
            __poll_input();
            break;
        }
    }

    return 1;
}