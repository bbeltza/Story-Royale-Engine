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

	engine.sdl_rendererhndl = sresdlrenderer_driver.renderer;

	int locked = SDL_TryLockMutex(engine.sdl_rendermutex);
	assert(!locked);

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

	engine.size_x = engine.osize_x / engine.viewport_scale;
	engine.size_y = engine.osize_y / engine.viewport_scale;

	engine.center_x = engine.size_x / 2.0_ut;
	engine.center_y = engine.size_y / 2.0_ut;

	engine.video->scale = engine.viewport_scale;
	engine.video->center_x = engine.center_x;
	engine.video->center_y = engine.center_y;
}

void __display_render()
{
	sre::onUpdate.Fire();

	// Unlock the renderer
	SDL_UnlockMutex(engine.sdl_rendermutex);

	// Render current world

	if (sreECS::Scene *current = static_cast<sreECS::Scene*>(engine.current_world))
	{
		engine.video->camera_x = current->camera.position.x;
		engine.video->camera_y = current->camera.position.y;

		//// Aliases for the background and the foreground (kind of old)
		const sre::col4 bg{current->background};
		const sre::col4& fg = current->foreground;

		//// Clearing the screen with the background color
		engine.video->draw_clear(reinterpret_cast<const sre_u8*>(&bg));

		sre::beforeRender.Fire();

		//// Drawing all the entities (doesn't run if the foreground is full opaque)
		if (fg.a < 255)
			current->call_render();

		//// Finally, filling the foreground (doesn't run if the foreground is invisible)
		if (fg.a)
			engine.video->draw_fill(reinterpret_cast<const sre_DDFill*>(&fg));
	}
	else
	{
		engine.video->camera_x = 0;
		engine.video->camera_y = 0;

		engine.video->draw_clear(NULL);
		sre::beforeRender.Fire();
	}

	// Drawing the Gui layer
	if (sreGUI::Object *current = currlayer)
		current->call_render();

	sre::afterRender.Fire();

	// Present the screen
	SDL_LockMutex(engine.sdl_rendermutex);

	engine.video->present();
}