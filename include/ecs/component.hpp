#ifndef SREECS_COMPONENT_HPP
#define SREECS_COMPONENT_HPP

namespace sreECS
{
	class Entity;

	struct Component
	{
		virtual ~Component() = 0;
		virtual void on_render(Entity* entity) {}
		virtual void on_update(Entity* entity) {}
		virtual void on_pupdate(Entity* entity) {}
		virtual void on_query(Entity* entity, const float* pt) {}
	};
}

#endif