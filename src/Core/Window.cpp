#include "../internal.h"

#include "Base/Window.hpp"

#include "Classes/File.hpp"

void Window::ChangeIcon(const char* path)
{
    File file;
    file.Load(path);

    SDL_Surface* icon = IMG_Load_RW(SDL_RWFromConstMem(file.getRawData(), (int)file.getSize()), 1);
    SDL_SetWindowIcon(engine.sdl_windowhndl, icon);
    SDL_FreeSurface(icon);
}

void Window::ToggleFullscreen()
{
    static int oW, oH; // Old width and height for the window
    bool willbe_full = !IsFullScreen();
    if (willbe_full)
    {
        SDL_DisplayMode d;
        SDL_GetDesktopDisplayMode(0, &d);
        SDL_SetWindowDisplayMode(engine.sdl_windowhndl, &d);
        SDL_GetWindowSize(engine.sdl_windowhndl, &oW, &oH);
    }

    SDL_SetWindowFullscreen(engine.sdl_windowhndl, willbe_full * SDL_WINDOW_FULLSCREEN_DESKTOP);
}

void Window::SetFramerate(unsigned short FPS)
{

}

void Window::Show() { SDL_ShowWindow(engine.sdl_windowhndl); }
void Window::Hide() { SDL_HideWindow(engine.sdl_windowhndl); }
void Window::Focus() { SDL_SetWindowInputFocus(engine.sdl_windowhndl); }

bool Window::IsFullScreen() { return SDL_GetWindowFlags(engine.sdl_windowhndl) & SDL_WINDOW_FULLSCREEN; }
bool Window::IsHidden() { return SDL_GetWindowFlags(engine.sdl_windowhndl) & SDL_WINDOW_HIDDEN; }