#include <standard>
#include "../internal.h"

#include "Game/GameInstance.hpp"

#include "Base/object.hpp"

static std::unordered_set<GameInstance*> destroy_queue;
static std::unordered_set<sre::Object*> destroy_queue2;

void __destroy_queue()
{
	SDL_LockMutex(engine.destroyqueue_mutex);
	while (!destroy_queue.empty()) delete *destroy_queue.begin();
	while (!destroy_queue2.empty()) delete *destroy_queue2.begin();
	SDL_UnlockMutex(engine.destroyqueue_mutex);
}

GameInstance::~GameInstance() { destroy_queue.erase(this); }
void GameInstance::Destroy()
{
	SDL_LockMutex(engine.destroyqueue_mutex);
	destroy_queue.insert(this);
	SDL_UnlockMutex(engine.destroyqueue_mutex);
}

sre::Object::~Object() { destroy_queue2.erase(this); }
void sre::Object::destroy()
{
	SDL_LockMutex(engine.destroyqueue_mutex);
	destroy_queue2.insert(this);
	SDL_UnlockMutex(engine.destroyqueue_mutex);
}