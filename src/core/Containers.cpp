#include "../internal.h"

#include "Base/Display.hpp"
#include "Core/Texture.hpp"
#include "Base/Thread.hpp"
#include "Base/Audio.hpp"

#include "ECS/scene.hpp"
#include "GUI/object.hpp"

void __clean_containers()
{
	if (engine.current_world)
		delete static_cast<::sreECS::Scene*>(engine.current_world);
	if (engine.current_guilayer)
		delete static_cast<::sreGUI::Object*>(engine.current_guilayer);
}

void __update_layer()
{
	begin:
	sreGUI::Object* current = currlayer;
	
	if (!current) return;
	current->m_absolute.size = sre::display_size();
	current->m_absolute.position = sre::vec2ut::ZERO;

	current->call_process();
	current->call_processchildren();
	current->call_prerender();

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
