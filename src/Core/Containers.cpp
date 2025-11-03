#include "../internal.h"

#include "Base/Texture.hpp"
#include "Base/Thread.hpp"
#include "Base/Audio.hpp"
#include "Base/Tween.hpp"
#include "Base/Timer.hpp"

#include "Game/World.hpp"
#include "Game/GuiLayer.hpp"

typedef std::tuple<
	std::list<Thread::data>,
	std::queue<Thread::data*>,

	std::unordered_map<std::string, std::unique_ptr<AudioData>>,
	std::unordered_set<Audio*>,

	std::vector<SDL_Texture*>
> _Tup;

static _Tup* containers_tuple;

#define get_container(n) std::get<n>(*containers_tuple)

void __init_containers()
{
	containers_tuple = new _Tup;

	engine.allocated_threads = &get_container(0);
	engine.finished_threads = &get_container(1);

	engine.loaded_audios = &get_container(2);
	engine.audio_queue = &get_container(3);

	engine.target_textures = &get_container(4);

	get_container(4).push_back(NULL);
}

void __clean_containers()
{
	memset(&engine.allocated_threads, 0, &engine.target_textures - &engine.allocated_threads);

	delete containers_tuple;

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