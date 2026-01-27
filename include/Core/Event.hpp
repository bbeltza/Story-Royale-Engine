#ifndef SRE_EVENT_HPP
#define SRE_EVENT_HPP

#include <Core/Input.hpp>
#include <Base/Signal.hpp>
#include <ints.h>

#include <Datatypes/Vector.hpp>

namespace sre
{
	enum eventType
	{
		EVENT_DEFAULT,

		EVENT_MOUSEBUTTON,
		EVENT_MOUSEWHEEL,
		EVENT_MOUSEMOVE,
		EVENT_KEYPRESS,
		EVENT_TOUCH
	};

	enum keyPress
	{
		KEY_RELEASED = 0,
		KEY_PRESSED = 1,
		KEY_REPEATED = 2
	};
}

namespace sre
{
	namespace events
	{
		struct Common
		{
			eventType type;
		};

		struct MouseButton: Common
		{
			sre::u32 id;
			sre::u8 button;
			sre::u8 clicks;
			bool pressed;
			bool : 1;

			sre::vec2ut position;
		};

		struct MouseWheel: Common
		{
			sre::u32 id;
			sre::vec2i amount;
			sre::vec2ut position;
		};

		struct MouseMove: Common
		{
			sre::u32 id;
			sre::u32 button_state;
			sre::vec2ut position;
			sre::vec2ut delta;
		};

		struct Key: Common
		{
			sre::keyCode keycode;
			sre::scanCode scancode;
			sre::keyMod keymod;
			u32 press;
		};

		struct Touch: Common
		{
			sre::vec2ut uv;
			sre::vec2ut delta;
			sre::unit pressure;
			bool pressed;
		};
	}
}

namespace sre
{
	union Event
	{
		Event() {}
		Event(const Event& copy) { memcpy(this, &copy, sizeof(Event)); };

		eventType type;
		events::Common common;

		events::MouseButton mouse_button;
		events::MouseWheel mouse_wheel;
		events::MouseMove mouse_move;

		events::Key key_press;

		events::Touch touch;
	};

	extern Signal<sre::Event> onEvent;
}

#endif