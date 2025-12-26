#include "../internal.h"
#include "../internal.hpp"

#include "Base/Texture.hpp"
#include "Base/Thread.hpp"
#include "Base/Audio.hpp"
#include "Base/Tween.hpp"
#include "Base/Timer.hpp"

#include "ECS/scene.hpp"
#include "GUI/object.hpp"

_containers_service::_containers_service()
{
}

_containers_service::~_containers_service()
{
	lock();

	// Destroy all concurrency-related stuff
	allocated_threads.clear();

	// Destroy SDL fonts (they also cannot be destroyed after)
	while (!loaded_fonts.empty())
	{
		auto& font = loaded_fonts.front();
		if (font->m_font)
		{
			TTF_CloseFont(font->m_font);
			font->m_font = NULL;
			font->m_src = NULL;
		}
		loaded_fonts.pop_front();
	}

	// Nothing much more for now..

	unlock();
}

void __init_containers()
{
	engine.containers_service = new _containers_service;
}

void __clean_containers()
{
	_containers_service* containers = _containers;
	engine.containers_service = NULL;
	delete containers;

	if (engine.current_world)
		delete static_cast<::sreECS::Scene*>(engine.current_world);
	if (engine.current_guilayer)
		delete static_cast<::sreGUI::Object*>(engine.current_guilayer);
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
	sreGUI::Object* current = currlayer;
	
	if (!current) return;
	current->m_absolute.size = sre::vec2ut{engine.viewport.w, engine.viewport.h};
	current->m_absolute.position = sre::vec2ut::ZERO;

	current->call_process();
	current->call_update();

	if (current != engine.current_guilayer) goto begin;
}

void __update_world()
{
	for (;;)
	{
		sreECS::Scene* current = static_cast<sreECS::Scene*>(engine.current_world);
	
		if (!current) return;
			current->call_update();
	
		if (current != engine.current_world) continue;
		break;
	};
}
