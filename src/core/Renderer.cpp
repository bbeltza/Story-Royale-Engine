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
	engine.video = static_cast<sre_videodriver*>(calloc(1, sizeof(sre_videodriver)));
	assert(engine.video);
	if (sresdlrenderer_init(engine.video, engine.sdl_windowhndl) < 0)
	{
		ERROR("Failed initializing the render driver");
		exit(-1);
	}

	if (engine.video->texture_size)
	{
		const_cast<void*&>(engine.video->textures) = calloc(SRE_TEXTURE_BASECOUNT, engine.video->texture_size);
		const_cast<sre_usize&>(engine.video->textures_capacity) = SRE_TEXTURE_BASECOUNT;

		const_cast<sre_u32* &>(engine.video->texture_fl) = static_cast<sre_u32*>(calloc(SRE_TEXTURE_BASECOUNT, sizeof(sre_u32)));
		const_cast<sre_usize&>(engine.video->texture_flcapacity) = SRE_TEXTURE_BASECOUNT;

		assert(engine.video->textures && engine.video->texture_fl);
	}

	if (sre::game_settings.WindowOptions.VSync)
		engine.video->vsync(engine.video, 1);

	int locked = SDL_TryLockMutex(engine.sdl_rendermutex);
	assert(!locked);

	sre::set_framerate(sre::game_settings.WindowOptions.TargetFPS);
}

void __update_viewport(int w, int h)
{
	int integer_scale;

	if (sre::game_settings.WindowOptions.Scaled)
	{
		integer_scale = ut_min(w / sre::game_settings.WindowOptions.Resolution.x, h / sre::game_settings.WindowOptions.Resolution.y);
		integer_scale = integer_scale ? integer_scale : 1;
	}
	else
	{
		integer_scale = 1;
	}

	sre::unit scale = static_cast<sre::unit>(integer_scale);
	sre::vec2ut size = sre::vec2ut{ w, h } / scale;

	sre::vec2ut center = size / 2.0_ut;

	engine.osize_x = w;
	engine.osize_y = h;
	engine.scale_ratio = 1 / scale;
	engine.video->scale = scale;
	engine.video->size = size;
	engine.video->center = center;

	if (engine.video->viewport)
		engine.video->viewport(engine.video, w, h);
}

void __display_render()
{
	sre::onUpdate.Fire();

	// Unlock the renderer
	SDL_UnlockMutex(engine.sdl_rendermutex);

	// Render current world

	if (sreECS::Scene *current = static_cast<sreECS::Scene*>(engine.current_world))
	{
		engine.video->camera = current->camera.position;

		//// Aliases for the background and the foreground (kind of old)
		const sre::col4 bg{current->background};
		const sre::col4& fg = current->foreground;

		//// Clearing the screen with the background color
		engine.video->draw_clear(engine.video, &bg);

		sre::beforeRender.Fire();

		//// Drawing all the entities (doesn't run if the foreground is full opaque)
		if (fg.a < 255)
			current->call_render();

		//// Finally, filling the foreground (doesn't run if the foreground is invisible)
		if (fg.a)
			engine.video->draw_fill(engine.video, reinterpret_cast<const sre_DDFill*>(&fg));
	}
	else
	{
		engine.video->camera.x = 0;
		engine.video->camera.y = 0;

		engine.video->draw_clear(engine.video, NULL);
		sre::beforeRender.Fire();
	}

	// Drawing the Gui layer
	if (sreGUI::Object *current = currlayer)
		current->call_render();

	sre::afterRender.Fire();

	// Present the screen
	SDL_LockMutex(engine.sdl_rendermutex);

	engine.video->present(engine.video);
}