#include <ECS/entity.hpp>
#include <ECS/scene.hpp>

using namespace sreECS;

Entity::Entity(sre::unit x, sre::unit y, long z): position(x, y), z_index(z)
{
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

	if (m_size < Scene::_Arena::SIZE)
		m_parent->m_freelist.push_back(this);
	
	for (auto it = m_parent->m_entities.begin(); it != m_parent->m_entities.end(); it++)
	{
		if (*it != this) continue;

		*it = m_parent->m_entities.back();
		m_parent->m_entities.pop_back();
		break;
	}
}

void Entity::operator delete(void* entity)
{
	static_cast<Entity*>(entity)->~Entity();
}

void Entity::setup_components(Component* const components[], size_t count)
{
	if (m_components)
	{
		WARN("setup_components(): Current entity already has components attached to it, they will be deattached");
		if (m_componentcount != count)
		{
			operator delete (m_components);
			m_components = static_cast<Component**>(operator new(sizeof(Component*) * count));
		}
	}
	else
	{
		m_components = static_cast<Component**>(operator new(sizeof(Component*) * count));
	}

	m_componentcount = count;

	for (size_t i = 0; i < count; i++)
	{
		m_components[i] = components[i];
		// Possible future on_attach(Entity*) component virtual method
		// components[i].on_attach(this);
	}
}
