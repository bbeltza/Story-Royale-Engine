#ifndef SREECS_COMPONENT_HPP
#define SREECS_COMPONENT_HPP

#include <Datatypes/Vector.hpp>
#include <Datatypes/Flags.hpp>

#include <imgui.h>

namespace sre
{
	struct RenderInterface;
}

namespace sreECS
{
	class Entity;

	struct Component
	{
		friend struct Scene;

		enum flagHelpers
		{
			F_ENABLED = 1,
			F_FULL = -2
		};

		// Set of custom flags free for use (Watch `GUI/Component.hpp`)
		// The first bit is reserved (`F_ENABLED`)
		sre::flagsptr flags{F_ENABLED};

		// TODO: Add common component base class? (struct that shares both GUI and ECS components)

			inline bool enabled() const { return flags.has(F_ENABLED); }
            inline void enable() { flags.toggle_on(F_ENABLED); }
            inline void disable() { flags.toggle_off(F_ENABLED); }
		//

		constexpr Component() = default;
		constexpr Component(sre::flagsptr::type flags): flags{flags, F_ENABLED} {}
	protected:
		virtual void on_render(Entity& entity) {}
		virtual void on_update(Entity& entity) {}
		virtual void on_pupdate(Entity& entity) {}
		virtual bool on_query(Entity& entity, sre::vec2ut pt) const { return false; }

	#ifndef IMGUI_DISABLE
		virtual void ImGuiUpdate() {}
	#endif
	};
}

#endif