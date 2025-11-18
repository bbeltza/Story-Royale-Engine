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
	std::queue<Audio*> stopped_audios;

	std::vector<SDL_Texture*> target_textures;

	std::list<TTF_Font*> loaded_fonts;
};

static ENGINE_CONTAINERS* cc;

SDL_atomic_t SR_NUM_ALLOCATIONS;

#if !defined(NDEBUG) && WIN32 // This operator 
void* operator new(size_t size)
{
	size_t newsize = size + sizeof(size_t);
	size_t* block = reinterpret_cast<size_t*>(malloc(newsize));
	if (!block)
		abort();

	block[0] = size;
	SDL_AtomicAdd(&SR_NUM_ALLOCATIONS, static_cast<int>(size));

	//LOG("GOT OPERATOR NEW, NOW CURRENT SIZE: %zd", SDL_AtomicGet(&SR_NUM_ALLOCATIONS));

	block++;
	return block;
}
void operator delete(void* block)
{
	if (!block) return;
	
	size_t* tblock = reinterpret_cast<size_t*>(block);
	tblock -= 1;

	SDL_AtomicAdd(&SR_NUM_ALLOCATIONS, -static_cast<int>(tblock[0]));

	//LOG("GOT OPERATOR DELETE, NOW CURRENT SIZE: %zd", SDL_AtomicGet(&SR_NUM_ALLOCATIONS));

	free(tblock);
}
#endif

void __init_containers()
{
	cc = new ENGINE_CONTAINERS;

	engine.allocated_threads = &cc->allocated_threads;
	engine.finished_threads = &cc->finished_threads;

	engine.loaded_audios = &cc->loaded_audios;
	engine.audio_queue = &cc->audio_queue;
	engine.stopped_audios = &cc->stopped_audios;

	engine.target_textures = &cc->target_textures;

	engine.loaded_fonts = &cc->loaded_fonts;

	cc->target_textures.push_back(NULL);
}

void __clean_containers()
{
	Thread::queue_removing();

	memset(&engine.allocated_threads, 0, 7);

	for (auto font : cc->loaded_fonts)
	{
		TTF_CloseFont(font);
	}

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
	current->call_update(engine.last_dt);
	current->call_pupdate(engine.last_dt);
	__destroy_queue();

	if (current != engine.current_world) goto begin;
}
