#include <standard>
#include "../internal.h"

#include "Game/GameInstance.hpp"

static std::queue<GameInstance*> destroy_queue;

void __destroy_queue()
{
	SDL_LockMutex(engine.destroyqueue_mutex);
	while (!destroy_queue.empty())
		delete destroy_queue.front();
	SDL_UnlockMutex(engine.destroyqueue_mutex);
}

GameInstance::~GameInstance() { destroy_queue.pop(); }
void GameInstance::Destroy()
{
	SDL_LockMutex(engine.destroyqueue_mutex);
	destroy_queue.push(this);
	SDL_UnlockMutex(engine.destroyqueue_mutex);
}