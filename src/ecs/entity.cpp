#include <ECS/Entity.hpp>
#include <ECS/Scene.hpp>
#include <Base/Log.h>

using namespace sreECS;

Entity::Entity(Scene* scene, Component* const* components, size_t component_count, sre::vec2ut pos, long z_index):
	position(pos), z_index(z_index) {
	
	if (scene) {
		scene->add_child(this);
	}

	setup_components(components, component_count);
}

Entity::~Entity()
{
	if (m_components)
	{
		/* Invalidate component properties */
		// size_t count = m_componentcount;
		Component** ptr = m_components;
		m_componentcount = 0;
		m_components = NULL;

		::operator delete(ptr);
	}
	
	for (auto it = m_parent->m_entities.begin(); it != m_parent->m_entities.end(); it++)
	{
		if (*it != this) continue;

		*it = m_parent->m_entities.back();
		m_parent->m_entities.pop_back();
		break;
	}
}

void Entity::setup_components(Component* const components[], size_t count)
{
	if (!components) {
		if (count)
			sre::log(SRE_LOG_WARN "Entity::setup_components(): `components` is NULL (meaning a request to detach all of the components is made), but `count` is not 0");

		::operator delete(m_components);
		m_components = NULL;
		m_componentcount = 0;
		return;
	}

	if (m_components)
	{
		sre::log(SRE_LOG_WARN "Entity::setup_components(): Current entity already has components attached to it, they will be deattached");
		if (m_componentcount != count)
		{
			operator delete (m_components);
			m_components = static_cast<Component**>(::operator new(sizeof(Component*) * count));
		}
	}
	else
	{
		m_components = static_cast<Component**>(::operator new(sizeof(Component*) * count));
	}

	m_componentcount = count;

	for (size_t i = 0; i < count; i++)
	{
		m_components[i] = components[i];
		// Possible future on_attach(Entity*) component virtual method
		// components[i].on_attach(this);
	}
}
