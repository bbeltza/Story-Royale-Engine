#ifndef SRE_EVENT_HPP
#define SRE_EVENT_HPP

#include <Core/Input.hpp>
#include <Base/Signal.hpp>
#include <ints.h>

#include <Datatypes/Vector.h>

namespace sre
{
	enum eventType
	{
		EVENT_DEFAULT,

		EVENT_MOUSEBUTTON, /* Use `sre::events::MouseButton` */
		EVENT_MOUSEWHEEL, /* `sre::events::MouseWheel` */
		EVENT_MOUSEMOVE, /* `sre::events::MouseMove` */
		EVENT_KEYPRESS, /* `sre::events::Key` */
		EVENT_TOUCH, /* `sre::events::Touch` */
		EVENT_QUIT /* No event struct associated, at least for now, might be an `sre::events::Quit` with an error code */
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
		struct MouseButton
		{
			static constexpr eventType EVENT_TYPE = EVENT_MOUSEBUTTON;

			sre::u32 id;
			sre::u8 button;
			sre::u8 clicks;
			bool pressed;
			bool : 1;

			sre::vec2ut position;
		};

		struct MouseWheel
		{
			static constexpr eventType EVENT_TYPE = EVENT_MOUSEWHEEL;

			sre::u32 id;
			sre::vec2i amount;
			sre::vec2ut position;
		};

		struct MouseMove
		{
			static constexpr eventType EVENT_TYPE = EVENT_MOUSEMOVE;

			sre::u32 id;
			sre::u32 button_state;
			sre::vec2ut position;
			sre::vec2ut delta;
		};

		struct Key
		{
			static constexpr eventType EVENT_TYPE = EVENT_KEYPRESS;

			sre::keyCode keycode;
			sre::scanCode scancode;
			sre::keyMod keymod;
			int press;
		};

		struct Touch
		{
			static constexpr eventType EVENT_TYPE = EVENT_TOUCH;

			sre::vec2ut uv;
			sre::vec2ut delta;
			sre::unit pressure;
			bool pressed;
		};
	}
}

namespace sre
{
	template <size_t n, typename _eventType=eventType>
	struct EventBase
	{
		#define __CHECK_EVSIZE static_assert(n >= sizeof(evT), "Should not happen, in case it happens, make Event::_data bigger");

		template <typename evT>
		inline EventBase(const evT& ev): m_type(evT::EVENT_TYPE)
		{
			__CHECK_EVSIZE
			new(_data) evT{ev};
		}

		template <typename evT>
		evT& get() {
			// Event structs don't need any inheritance, they instead just need to define the static constant EVENT_TYPE
			// This would also mean that you can make your own events, if they're smaller than Event's set constant capacity.
			//		It is still not recommended, making custom events for custom systems is possible, but this is only meant to be used with sre::onEvent
			__CHECK_EVSIZE

			static constexpr int evtype = evT::EVENT_TYPE;
			if (m_type == evtype)
				return reinterpret_cast<evT&>(*this->_data);
			
			throw std::bad_cast();
		}

		template <typename evT>
		const evT& get() const {
			__CHECK_EVSIZE

			static constexpr int evtype = evT::EVENT_TYPE;
			if (m_type == evtype)
				return reinterpret_cast<const evT&>(*this->_data);
			
			throw std::bad_cast();
		}

		template <typename evT>
		operator evT&() { return get<evT>(); }

		_eventType type() const { return m_type; }
		private:
			const _eventType m_type;
			char _data[n];
	};

	using Event = EventBase<28>;

	extern Signal<sre::Event> onEvent;
}

#endif