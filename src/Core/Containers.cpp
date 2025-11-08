#include "../internal.h"

#include "Base/Texture.hpp"
#include "Base/Thread.hpp"
#include "Base/Audio.hpp"
#include "Base/Tween.hpp"
#include "Base/Timer.hpp"

#include "Game/World.hpp"
#include "Game/GuiLayer.hpp"

struct ENGINE_CONTAINERS
{
	std::list<Thread::data> allocated_threads;
	std::queue<Thread::data*> finished_threads;

	std::unordered_map<std::string, std::unique_ptr<AudioData>> loaded_audios;
	std::unordered_set<Audio*> audio_queue;

	std::vector<SDL_Texture*> target_textures;
};

static ENGINE_CONTAINERS* cc;

#define get_container(n) std::get<n>(*containers_tuple)

void __init_containers()
{
	cc = new ENGINE_CONTAINERS;

	engine.allocated_threads = &cc->allocated_threads;
	engine.finished_threads = &cc->finished_threads;

	engine.loaded_audios = &cc->loaded_audios;
	engine.audio_queue = &cc->audio_queue;

	engine.target_textures = &cc->target_textures;

	cc->target_textures.push_back(NULL);
}

void __clean_containers()
{
	Thread::queue_removing();

	memset(&engine.allocated_threads, 0, (&engine.target_textures - &engine.allocated_threads) * sizeof(void*));

	delete cc;

	if (engine.current_world)
		delete reinterpret_cast<::Game::World*>(__engine_data.current_world);
	if (engine.current_guilayer)
		delete reinterpret_cast<::Game::GuiContainer*>(__engine_data.current_guilayer);

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
	if (!engine.current_guilayer) return;
	currlayer->m_absolute.Size.X = static_cast<float>(engine.viewport.w);
	currlayer->m_absolute.Size.Y = static_cast<float>(engine.viewport.h);
	currlayer->_processchildren();

	currlayer->call_update(engine.last_dt);
}

void __update_world()
{
	if (!engine.current_world) return;
	currworld->call_update(engine.last_dt);
	currworld->call_pupdate(engine.last_dt);
}