#include <standard>
#include "../internal.h"

#include "Base/object.hpp"

#include <utils/logging.h>

static void* const PTR_MAX = reinterpret_cast<void*>(UINTPTR_MAX);
static sre::Object* obj_head = static_cast<sre::Object*>(PTR_MAX); // PTR_MAX to make sure an object is in a queue, otherwise, m_nextdestroyed is NULL
static size_t queue_count = 0;

void __destroy_queue()
{
	SDL_LockMutex(engine.destroyqueue_mutex);
	while (obj_head != PTR_MAX)
		delete obj_head;
	SDL_UnlockMutex(engine.destroyqueue_mutex);
}

sre::Object::~Object() 
{
	if (obj_head == this)
	{
		obj_head = m_nextdestroyed;
		//queue_count--;
	}
}
void sre::Object::destroy()
{
	if (m_nextdestroyed) // Object is already in the queue, don't insert it again
		return;

	SDL_LockMutex(engine.destroyqueue_mutex);
	m_nextdestroyed = obj_head;
	obj_head = this;
	//queue_count++;
	SDL_UnlockMutex(engine.destroyqueue_mutex);
}