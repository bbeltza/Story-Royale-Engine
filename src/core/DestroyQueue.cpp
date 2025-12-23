#include <standard>
#include "../internal.h"

#include "Base/object.hpp"

#include <utils/logging.h>

static sre::Object* obj_head = NULL;
//static size_t queue_count = 0;

void __destroy_queue()
{
	SDL_LockMutex(engine.destroyqueue_mutex);
	while (obj_head)
		obj_head->on_destroy();
	SDL_UnlockMutex(engine.destroyqueue_mutex);
}

sre::Object::~Object() 
{
	if (m_nextdestroyed || obj_head == this)
		obj_head = m_nextdestroyed;
	//queue_count--;
}
void sre::Object::destroy()
{
	if (m_nextdestroyed || obj_head == this)
		return; // Object is already in the queue, don't insert it again

	SDL_LockMutex(engine.destroyqueue_mutex);
	m_nextdestroyed = obj_head;
	obj_head = this;
	//queue_count++;
	SDL_UnlockMutex(engine.destroyqueue_mutex);
}