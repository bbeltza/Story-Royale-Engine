#include "../internal.h"

#include "GameSettings.hpp"

#include "Game/World.hpp"
#include "Game/GuiLayer.hpp"

#include "Engine.hpp"

#include "Base/Texture.hpp"

#define t_textures reinterpret_cast<std::vector<SDL_Texture*>*>(engine.target_textures)

static void reset_targets()
{
	while (t_textures->size() > 1)
	{
		SDL_DestroyTexture(t_textures->back());
		t_textures->pop_back();
	}
}

void __setup_renderer(uint32_t flags)
{
	engine.sdl_rendererhndl = SDL_CreateRenderer(engine.sdl_windowhndl, -1, flags);
	SDL_SetRenderDrawBlendMode(engine.sdl_rendererhndl, SDL_BLENDMODE_BLEND);

	engine.sdl_rectTex = SDL_CreateTexture(engine.sdl_rendererhndl, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STATIC, 1, 1);
	{
		uint64_t WHITE = UINT64_MAX;
		SDL_UpdateTexture(engine.sdl_rectTex, NULL, &WHITE, 4);
	}

	SDL_TryLockMutex(engine.sdl_rendermutex);

	Runtime::SetFramerate(GameSettings::TargetFPS);
}

void __update_viewport()
{
	static bool has_scaling = GameSettings::ScalingResolution.operator bool();

	SDL_GetRendererOutputSize(engine.sdl_rendererhndl, &engine.osize_x, &engine.osize_y);
	if (has_scaling)
	{
		engine.integer_scale = ut_min(engine.osize_x / GameSettings::ScalingResolution.X, engine.osize_y / GameSettings::ScalingResolution.Y);
		engine.integer_scale = engine.integer_scale ? engine.integer_scale : 1;
	}
	else
		engine.integer_scale = 1;
	{
		float fscale = static_cast<float>(engine.integer_scale);
		SDL_RenderSetScale(engine.sdl_rendererhndl, fscale, fscale);
	}

	int ow = engine.viewport.w;
	int oh = engine.viewport.h;
	SDL_RenderGetViewport(engine.sdl_rendererhndl, &engine.viewport);
	if (ow < engine.viewport.w || oh < engine.viewport.h)
		reset_targets(); // Later on for resetting target textures
	engine.center_x = engine.viewport.w / 2.0f;
	engine.center_y = engine.viewport.h / 2.0f;
}

void __display_render()
{
	Runtime::OnUpdate.Fire(engine.last_dt);

	// Load unloaded textures
	Texture::load_textures();

	// Unlock the renderer
	SDL_UnlockMutex(engine.sdl_rendermutex);

	// Render current world

	if (Game::World* current = currworld)
	{
		//// Aliases for the background and the foreground (so that typing Game::currentWorld wouldn't be necessary)
		Color3& bg = current->Background;
		Color4& fg = current->Foreground;

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
	if (Game::GuiLayer* current = currlayer)
	{
		Color4& fg = current->Foreground;

		if (fg.a < 255)
		{
			current->preRender();
			current->_renderchildren();
			current->postRender();
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

void Texture::load_textures()
{
	if (!to_load) return;
	while (!to_load->empty())
	{
		Texture* tex = to_load->back();
		to_load->pop_back();
		tex->texture = SDL_CreateTextureFromSurface(engine.sdl_rendererhndl, reinterpret_cast<SDL_Surface*>(tex->file_surface));
	}
}