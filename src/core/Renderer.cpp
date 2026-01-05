#include "../internal.h"
#include "../internal.hpp"
#include <sdl_renderer/sdl_renderer.h>

#include <GameSettings.h>

#include <Base/Texture.hpp>
#include <Base/Runtime.hpp>

#include <ECS/scene.hpp>
#include <GUI/object.hpp>

void __setup_renderer()
{
	engine.video = sresdlrenderer_init(engine.sdl_windowhndl);

	if (sre::game_settings.WindowOptions.VSync)
		engine.video->vsync(1);

	engine.sdl_rendererhndl = sre_SDLRendererDriver.renderer;
	engine.sdl_rectTex = sre_SDLRendererDriver.rect_tex;

	SDL_TryLockMutex(engine.sdl_rendermutex);

	sre::set_framerate(sre::game_settings.WindowOptions.TargetFPS);
}

void __update_viewport()
{
	int integer_scale;

	SDL_GetRendererOutputSize(engine.sdl_rendererhndl, &engine.osize_x, &engine.osize_y);
	if (sre::game_settings.WindowOptions.Scaled)
	{
		integer_scale = ut_min(engine.osize_x / sre::game_settings.WindowOptions.Resolution.x, engine.osize_y / sre::game_settings.WindowOptions.Resolution.y);
		integer_scale = integer_scale ? integer_scale : 1;
	}
	else
	{
		integer_scale = 1;
	}

	engine.viewport_scale = static_cast<sre::unit>(integer_scale);
	engine.current_scale = engine.viewport_scale;

	sre::unit ow = engine.size_x;
	sre::unit oh = engine.size_y;
	(void)ow;
	(void)oh;

	engine.size_x = engine.osize_x / engine.viewport_scale;
	engine.size_y = engine.osize_y / engine.viewport_scale;

	engine.center_x = engine.size_x / 2.0_ut;
	engine.center_y = engine.size_y / 2.0_ut;
}

void __display_render()
{
	sre::onUpdate.Fire();

	// Unlock the renderer
	SDL_UnlockMutex(engine.sdl_rendermutex);

	// Render current world

	if (sreECS::Scene *current = static_cast<sreECS::Scene*>(engine.current_world))
	{
		//// Aliases for the background and the foreground (so that typing Game::currentWorld wouldn't be necessary)
		const sre::col3& bg = current->background;
		const sre::col4& fg = current->foreground;

		//// Clearing the screen with the background color
		SDL_SetRenderDrawColor(engine.sdl_rendererhndl, bg.r, bg.g, bg.b, 255);
		SDL_RenderClear(engine.sdl_rendererhndl);

		sre::beforeRender.Fire();

		//// Drawing all the entities (doesn't run if the foreground is full opaque)
		if (fg.a < 255)
			current->call_render();

		//// Finally, filling the foreground (doesn't run if the foreground is invisible)
		// printf("%u, %u, %u, %u / %u, %u, %u\n", fg.r, fg.g, fg.b, fg.a, bg.r, bg.g, bg.b);
		if (fg.a)
		{
			SDL_SetRenderDrawColor(engine.sdl_rendererhndl, fg.r, fg.g, fg.b, fg.a);
			SDL_RenderFillRect(engine.sdl_rendererhndl, NULL);
		}
	}
	else
	{
		SDL_SetRenderDrawColor(engine.sdl_rendererhndl, 0, 0, 0, 0);
		SDL_RenderClear(engine.sdl_rendererhndl);
		sre::beforeRender.Fire();
	}

	// Drawing the Gui layer
	if (sreGUI::Object *current = currlayer)
		current->call_render();

	sre::afterRender.Fire();

	// Present the screen
	SDL_LockMutex(engine.sdl_rendermutex);

	SDL_RenderPresent(engine.sdl_rendererhndl);
}