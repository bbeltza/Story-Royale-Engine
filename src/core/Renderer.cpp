#include "../internal.h"
#include "../internal.hpp"

#include "GameSettings.h"

#include "Game/GuiLayer.hpp"

#include "Engine.hpp"

#include "Base/Texture.hpp"

#include "ECS/scene.hpp"

void __setup_renderer()
{
	uint32_t flags = 0;
	if (sre::game_settings.WindowOptions.VSync)
		flags |= SDL_RENDERER_PRESENTVSYNC;

	engine.sdl_rendererhndl = SDL_CreateRenderer(engine.sdl_windowhndl, -1, flags);
	SDL_SetRenderDrawBlendMode(engine.sdl_rendererhndl, SDL_BLENDMODE_BLEND);

	engine.sdl_rectTex = SDL_CreateTexture(engine.sdl_rendererhndl, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STATIC, 1, 1);
	{
		uint64_t WHITE = UINT64_MAX;
		SDL_UpdateTexture(engine.sdl_rectTex, NULL, &WHITE, 4);
	}

	SDL_TryLockMutex(engine.sdl_rendermutex);

	Runtime::SetFramerate(sre::game_settings.WindowOptions.TargetFPS);
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

	engine.viewport_scale = static_cast<float>(integer_scale);
	engine.current_scale = engine.viewport_scale;

	int ow = engine.viewport.w;
	int oh = engine.viewport.h;
	(void)ow;
	(void)oh;

	engine.viewport.w = (int)(engine.osize_x / engine.viewport_scale);
	engine.viewport.h = (int)(engine.osize_y / engine.viewport_scale);

	engine.center_x = engine.viewport.w / 2.0f;
	engine.center_y = engine.viewport.h / 2.0f;
}

void __display_render()
{
	Runtime::OnUpdate.Fire(engine.last_dt);

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

		Runtime::BeforeRender.Fire();

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
		Runtime::BeforeRender.Fire();
	}

	// Drawing the Gui layer
	if (Game::GuiLayer *current = currlayer)
	{
		sre::col4& fg = current->Foreground;

		if (fg.a < 255)
		{
			current->pre_render();
			current->_renderchildren();
			current->post_render();
		}
		if (fg.a)
		{
			SDL_SetRenderDrawColor(engine.sdl_rendererhndl, fg.r, fg.g, fg.b, fg.a);
			SDL_RenderFillRect(engine.sdl_rendererhndl, NULL);
		}
	}

	Runtime::AfterRender.Fire();

	// Present the screen
	SDL_LockMutex(engine.sdl_rendermutex);

	SDL_RenderPresent(engine.sdl_rendererhndl);
}