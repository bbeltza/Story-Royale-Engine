#include "../internal.h"
#include "GameSettings.hpp"
#include "Datatypes/Flags.hpp"

struct _win_settings __setup_window_data()
{
	Vector2i start_res(GameSettings::StartResolution ? GameSettings::StartResolution : GameSettings::ScalingResolution * 2);
	Flags32 win_flags{};
	Flags32 render_flags;
	if (GameSettings::WindowOptions.Resizable)
		win_flags.ToggleOn(SDL_WINDOW_RESIZABLE);
	if (GameSettings::WindowOptions.Hidden)
		win_flags.ToggleOn(SDL_WINDOW_HIDDEN);

	if (GameSettings::WindowOptions.VSync)
		render_flags.ToggleOn(SDL_RENDERER_PRESENTVSYNC);

	return { GameSettings::Title, start_res.X, start_res.Y, win_flags.Get(), render_flags.Get() };
}