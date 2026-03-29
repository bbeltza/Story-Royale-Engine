#include "../internal.h"

#include <Core/Object.hpp>
#include <Core/Display.hpp>

#include <ECS/Scene.hpp>
#include <GUI/Object.hpp>

#include <cassert>

static void* const PTR_MAX = reinterpret_cast<void*>(UINTPTR_MAX); 
static sre::Object* obj_head = static_cast<sre::Object*>(PTR_MAX); // PTR_MAX to make sure an object is in a queue, otherwise, m_nextdestroyed is NULL
//static size_t queue_count = 0;

namespace sre
{
    class ECS;
}
class sre::ECS
{
    ECS() = delete;

    static inline void update_scene();
    static inline void update_layer();
    public:
        static void destroy_queue();

        static void call_query();

        static void call_update()
        {
            update_scene();
            update_layer();
        }
        static void render_ui();
        static void render_scene();
};

//
sre::Object::~Object() 
{
    assert(SDL_TryLockMutex(engine.destroyqueue_mutex) != SDL_MUTEX_TIMEDOUT);
	if (obj_head == this)
	{
		obj_head = m_nextdestroyed;
		//queue_count--;
	}
    assert(SDL_UnlockMutex(engine.destroyqueue_mutex) || 1);
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

void sre::ECS::destroy_queue()
{
    SDL_LockMutex(engine.destroyqueue_mutex);
    while (obj_head != PTR_MAX)
        delete obj_head;
    SDL_UnlockMutex(engine.destroyqueue_mutex);
}

//

void sre::ECS::call_query()
{
    if (!engine.current_guilayer && !engine.current_world) return;

    sre::vec2ut pt{engine.mouse_x, engine.mouse_y};

    if (engine.input_last_touchid < 0) goto callsection;
    {
        int finger_count = SDL_GetNumTouchFingers(engine.input_last_touchid);
        if (!finger_count) goto callsection;
    
        SDL_Finger* lastfinger = SDL_GetTouchFinger(engine.input_last_touchid, finger_count - 1);

        pt = sre::display_size() * sre::vec2ut{ lastfinger->x, lastfinger->y };
    }

    callsection:

    if (
        engine.current_guilayer && currlayer->call_query(pt) &&
        engine.current_world && currscn->call_query(pt)
    )
        (void)0;
}

//

void sre::ECS::update_scene()
{
    BEGIN:

	sreECS::Scene* current = static_cast<sreECS::Scene*>(engine.current_world);
	
	if (!current) return;
		current->call_update();
	
	if (current != engine.current_world) goto BEGIN;
}

void sre::ECS::update_layer()
{
    BEGIN:
	sreGUI::Object* current = currlayer;
	
	if (!current) return;
    sre::unit insets = sreGUI::get_insets();
	current->m_absolute.size = sre::display_size() - vec2ut{insets*2, 0};
	current->m_absolute.position = sre::vec2ut::ZERO + vec2ut{insets, 0};
	
    current->call_update();

	current->call_process();
	current->call_processchildren();
	current->call_prerender();

	if (current != engine.current_guilayer) goto BEGIN;
}

//

void __render_ui() { sre::ECS::render_ui(); }
void __render_scene() { sre::ECS::render_scene(); }

void sre::ECS::render_ui()
{
    if (engine.current_guilayer)
        currlayer->call_render();
}

void sre::ECS::render_scene()
{
    assert(engine.current_world != NULL);
    currscn->call_render();
}

//

void __update_ecs()
{
    sre::ECS::destroy_queue();
    sre::ECS::call_query();
    sre::ECS::call_update();
    sre::ECS::destroy_queue();
}

void __cleanup_ecs()
{
    SDL_LockMutex(engine.destroyqueue_mutex);
	    delete static_cast<::sreECS::Scene*>(engine.current_world);
	    delete static_cast<::sreGUI::Object*>(engine.current_guilayer);
    SDL_UnlockMutex(engine.destroyqueue_mutex);
}