#include <Core/Render.h>
#include <Core/Object.hpp>
#include <Core/Display.hpp>
#include <Core/Runtime.hpp>

#include "../internal.h"

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
            sre::onUpdate.fire();
        }
        static bool call_render();
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

extern void __render_clearqueues();

bool sre::ECS::call_render()
{
    if (SDL_GetWindowFlags(engine.sdl_windowhndl) & SDL_WINDOW_HIDDEN)
        return false;

#ifndef IMGUI_DISABLE
	auto imgui = engine.video->imgui;
	if (imgui)
	{
		imgui->imgui_newframe();
		ImGui_ImplSDL2_NewFrame();
	}
	else
		ImGui_ImplNull_NewFrame();
	ImGui::NewFrame();
#endif

    // Render current world

    sre::render_clipreset();

    float bg[3];
    if (sreECS::Scene *current = static_cast<sreECS::Scene*>(engine.current_world))
    {
    	bg[0] = current->background.r / 255.0f;
    	bg[1] = current->background.g / 255.0f;
    	bg[2] = current->background.b / 255.0f;

    	//// Aliases for the background and the foreground (kind of old)
    	const sre::col4& fg = current->foreground;

    #ifndef IMGUI_DISABLE
    	ImGui::Begin("Current scene"); {
    		float colb[4] = { current->background.r/255.0f, current->background.g/255.0f, current->background.b/255.0f, current->background.a/255.0f };
    		if (ImGui::ColorEdit4("Background", colb))
    			current->background = sre::col4::fromNormalized(colb[0], colb[1], colb[2], colb[3]);
        
    		float colf[4] = { fg.r/255.0f, fg.g/255.0f, fg.b/255.0f, fg.a/255.0f };
    		if (ImGui::ColorEdit4("Foreground", colf))
    			current->foreground = sre::col4::fromNormalized(colf[0], colf[1], colf[2], colf[3]);
    	}
    	ImGui::End();
    #endif

    	//// Clearing the screen with the background color

    	sre::beforeRender.fire();

    	//// Drawing all the entities (doesn't run if the foreground is full opaque)
    	if (fg.a < 255)
    		sre::ECS::render_scene();

    	//// Finally, filling the foreground (doesn't run if the foreground is invisible)
    	if (fg.a)
    		sre::render_fill(fg);
    }
    else
    {
		#if 0 // Debug easier on black backgrounds
			bg[0] = 0.3f;
			bg[1] = 0.3f;
			bg[2] = 0.3f;
		#else
			bg[0] = 0.0f;
    		bg[1] = 0.0f;
    		bg[2] = 0.0f;
    	#endif
    	sre::beforeRender.fire();
    }

	// Draw the GUI layer
	sre::ECS::render_ui();

	sre::afterRender.fire();

    return true;
}

//

bool __update_ecs()
{
    sre::ECS::destroy_queue();
    sre::ECS::call_query();
    sre::ECS::call_update();
    sre::ECS::destroy_queue();
    bool wantrender = sre::ECS::call_render();
    return wantrender;
}

void __cleanup_ecs()
{
    SDL_LockMutex(engine.destroyqueue_mutex);
	    delete static_cast<::sreECS::Scene*>(engine.current_world);
	    delete static_cast<::sreGUI::Object*>(engine.current_guilayer);
    SDL_UnlockMutex(engine.destroyqueue_mutex);
}