#include "Base/Window.h"

#include "Classes/File.h"

#include "Datatypes/Vector.h"
#include "Datatypes/Flags.h"

#include "GameSettings.h"

#define WCENTERED SDL_WINDOWPOS_CENTERED

static Vector2u start_res = GameSettings::StartResolution ? GameSettings::StartResolution : GameSettings::ScalingResolution * 2;
static Flags32 windowFlags = SDL_WINDOW_ALLOW_HIGHDPI;

WindowClass::WindowClass(EngineClass* engine): m_Engine(engine)
{
    if (GameSettings::WindowOptions.Resizable)
        windowFlags.ToggleOn(SDL_WINDOW_RESIZABLE);
    if (GameSettings::WindowOptions.Hidden)
        windowFlags.ToggleOn(SDL_WINDOW_HIDDEN);
}

void WindowClass::Setup()
{
    sdl_window = SDL_CreateWindow(
        GameSettings::Title,
        WCENTERED,
        WCENTERED,
        start_res.X,
        start_res.Y,
        windowFlags
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