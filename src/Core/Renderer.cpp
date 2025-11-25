#include "../internal.h"
#include "../internal.hpp"

#include "GameSettings.hpp"

#include "Game/World.hpp"
#include "Game/GuiLayer.hpp"

#include "Engine.hpp"

#include "Base/Texture.hpp"

static void reset_targets()
{
	_containers->lock();

	while (_containers->target_textures.size() > 1)
	{
		SDL_DestroyTexture(_containers->target_textures.back());
		_containers->target_textures.pop_back();
	}

	_containers->unlock();
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
	int integer_scale;

	SDL_GetRendererOutputSize(engine.sdl_rendererhndl, &engine.osize_x, &engine.osize_y);
	if (has_scaling)
	{
		integer_scale = ut_min(engine.osize_x / GameSettings::ScalingResolution.X, engine.osize_y / GameSettings::ScalingResolution.Y);
		integer_scale = integer_scale ? integer_scale : 1;
	}
	else
	{
		integer_scale = 1;
	}

	engine.viewport_scale = static_cast<float>(integer_scale);

	int ow = engine.viewport.w;
	int oh = engine.viewport.h;
	engine.viewport.w = (int)(engine.osize_x / engine.viewport_scale);
	engine.viewport.h = (int)(engine.osize_y / engine.viewport_scale);

	SDL_RenderSetScale(engine.sdl_rendererhndl, engine.viewport_scale, engine.viewport_scale);

	if (ow < engine.viewport.w || oh < engine.viewport.h)
		reset_targets();
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

	if (Game::World *current = currworld)
	{
		//// Aliases for the background and the foreground (so that typing Game::currentWorld wouldn't be necessary)
		Color3 &bg = current->Background;
		Color4 &fg = current->Foreground;

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
		Color4 &fg = current->Foreground;

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
	if (!to_load)
		return;
	_containers->lock();
	while (!to_load->empty())
	{
		Texture *tex = to_load->back();
		to_load->pop_back();
		tex->texture = SDL_CreateTextureFromSurface(engine.sdl_rendererhndl, reinterpret_cast<SDL_Surface *>(tex->file_surface));
		_containers->loaded_textures.push_front(tex);
	}
	_containers->unlock();
}