#include "Base/Window.h"

#include "Classes/File.h"

#include "GameSettings.h"

#define WCENTERED SDL_WINDOWPOS_CENTERED

WindowClass::WindowClass(EngineClass* engine): m_Engine(engine),
    start_res(GameSettings::StartResolution ? GameSettings::StartResolution : GameSettings::ScalingResolution * 2),
    start_flags(SDL_WINDOW_ALLOW_HIGHDPI)
{
    if (GameSettings::WindowOptions.Resizable)
        start_flags.ToggleOn(SDL_WINDOW_RESIZABLE);
    if (GameSettings::WindowOptions.Hidden)
        start_flags.ToggleOn(SDL_WINDOW_HIDDEN);
}

void WindowClass::Setup()
{
    sdl_window = SDL_CreateWindow(
        GameSettings::Title,
        WCENTERED,
        WCENTERED,
        start_res.X,
        start_res.Y,
        start_flags
    );
}

void WindowClass::setIcon(const char* path)
{
    File file;
    file.Load(path);

    SDL_Surface* icon = IMG_Load_RW(SDL_RWFromConstMem(file.getRawData(), file.getSize()), 1);
    SDL_SetWindowIcon(sdl_window, icon);
    SDL_FreeSurface(icon);
}