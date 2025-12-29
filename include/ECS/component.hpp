#ifndef SREECS_COMPONENT_HPP
#define SREECS_COMPONENT_HPP

#include <datatypes/vector.hpp>

namespace sreECS
{
	class Entity;

	struct Component
	{
		friend struct Scene;
		bool enabled = true;
	protected:
		virtual ~Component() {}
		virtual void on_render(Entity& entity) {}
		virtual void on_update(Entity& entity) {}
		virtual void on_pupdate(Entity& entity) {}
		virtual bool on_query(Entity& entity, sre::vec2ut pt) const { return false; }
	};
}

#endif