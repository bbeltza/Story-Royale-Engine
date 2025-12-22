#include <standard>
#include "../internal.h"

#include "Game/GameInstance.hpp"

#include "Base/object.h"

static std::unordered_set<GameInstance*> destroy_queue;

void __destroy_queue()
{
	SDL_LockMutex(engine.destroyqueue_mutex);
	while (!destroy_queue.empty()) delete *destroy_queue.begin();
	SDL_UnlockMutex(engine.destroyqueue_mutex);
}

GameInstance::~GameInstance() { destroy_queue.erase(this); }
void GameInstance::Destroy()
{
	SDL_LockMutex(engine.destroyqueue_mutex);
	destroy_queue.insert(this);
	SDL_UnlockMutex(engine.destroyqueue_mutex);
}