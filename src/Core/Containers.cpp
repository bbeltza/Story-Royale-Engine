#include "../internal.h"
#include "../internal.hpp"

#include "Base/Texture.hpp"
#include "Base/Thread.hpp"
#include "Base/Audio.hpp"
#include "Base/Tween.hpp"
#include "Base/Timer.hpp"

#include "Game/World.hpp"
#include "Game/GuiLayer.hpp"

_containers_service::_containers_service()
{
	target_textures.push_back(NULL);
}

_containers_service::~_containers_service()
{
	lock();

	// Destroy all concurrency-related stuff
	allocated_threads.clear();

	// Destroy SDL textures (they cannot be destroyed after SDL_Quit())
	while (target_textures.size() > 1)
	{
		SDL_DestroyTexture(target_textures.back());
		target_textures.pop_back();
	}
	while (!loaded_textures.empty())
	{
		auto& texture = loaded_textures.front();
		if (texture->texture)
		{
			SDL_DestroyTexture(static_cast<SDL_Texture*>(texture->texture));
			texture->texture = NULL;
		}
		loaded_textures.pop_front();
	}

	// Destroy SDL fonts (they also cannot be destroyed after)
	while (!loaded_fonts.empty())
	{
		auto& font = loaded_fonts.front();
		if (font->m_font)
		{
			TTF_CloseFont(font->m_font);
			font->m_font = NULL;
			font->m_src = NULL;
		}
		loaded_fonts.pop_front();
	}

	// Nothing much more for now..

	unlock();
}

void __init_containers()
{
	engine.containers_service = new _containers_service;
}

void __clean_containers()
{
	_containers_service* containers = _containers;
	engine.containers_service = NULL;
	delete containers;

	if (engine.current_world)
		delete static_cast<::Game::World*>(engine.current_world);
	if (engine.current_guilayer)
		delete static_cast<::Game::GuiContainer*>(engine.current_guilayer);

	__destroy_queue();
}

void __update_classes()
{
	Thread::queue_removing();

	engine.last_dt = Timer::global_update();

	TweenBase::global_update(engine.last_dt);
}

void __update_layer()
{
	begin:
	Game::GuiLayer* current = currlayer;
	
	if (!current) return;
	current->m_absolute.Size.X = static_cast<float>(engine.viewport.w);
	current->m_absolute.Size.Y = static_cast<float>(engine.viewport.h);
	current->_processchildren();
	
	current->call_update(engine.last_dt);
	__destroy_queue();

	if (current != engine.current_guilayer) goto begin;
}

void __update_world()
{
	begin:
	Game::World* current = currworld;
	
	if (!current) return;
	current->call_update();
	current->call_pupdate();
	
	__destroy_queue();
	if (current != engine.current_world) goto begin;
}
