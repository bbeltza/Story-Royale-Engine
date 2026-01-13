#ifndef SRE_ACTION_HPP
#define SRE_ACTION_HPP

#include <Base/Input.hpp>
#include <Base/Runtime.hpp>

namespace sre
{
	class Action
	{
		std::vector<unsigned> m_inputs;
		int m_counter = 0;
		int m_frame = -1;

		Action* m_next = NULL;
		Action* m_prev = NULL;
	public:
		Action();
		Action(const Action& other): Action() { m_inputs = other.m_inputs; }
		template <typename... Args> Action(Args... inputs): Action() { add_withargs(inputs...); }

		~Action();

		inline void add(mouseButton button) { return add_impl(static_cast<int>(button), C_MOUSE); }
		inline void add(scanCode button) { return add_impl(static_cast<int>(button), C_SCANCODE); }
		inline void add(keyCode button) { return add_impl(static_cast<int>(button), C_KEYCODE); }
		inline void remove(mouseButton button) { return remove_impl(static_cast<int>(button), C_MOUSE); }
		inline void remove(scanCode button) { return remove_impl(static_cast<int>(button), C_SCANCODE); }
		inline void remove(keyCode button) { return remove_impl(static_cast<int>(button), C_KEYCODE); }

		template <class T> inline void add(T) { static_assert(0, "Type must be either mouseButton, or scanCode, or keyCode"); }
		template <class T> inline void remove(T) { static_assert(0, "Type must be either mouseButton, or scanCode, or keyCode"); }

		bool pressed() const { return !(m_frame < 0); }
		bool just_pressed() const { return m_frame == sre::current_frame(); }

	private:
		enum
		{
			C_MOUSE,
			C_SCANCODE,
			C_KEYCODE
		};

		static constexpr int C_MOUSE_SHIFT = 0;
		static constexpr int C_MOUSE_MASK = 0b111;
		static constexpr int C_SCANCODE_SHIFT = 3;
		static constexpr int C_SCANCODE_MASK = 0x1FF;
		static constexpr int C_KEYCODE_SHIFT = 12;
		static constexpr int C_KEYCODE_MASK = C_SCANCODE_MASK;


		void add_impl(int value, int category);
		void remove_impl(int value, int category);

		template <typename T, typename... Args> inline void add_withargs(T first, Args... inputs) { add(first); return add_withargs(inputs...); }
		template <typename T> inline void add_withargs(T first) { add(first); }

		static void impl_getshiftmask(int category, int& shift, int& mask);
	
	private:
		static Action* head_ptr;
		static ConnectionHandle sc_event;
		static ConnectionHandle sc_mouse;
		static ConnectionHandle sc_keyboard;
		static ConnectionHandle sc_touch;
		static void sc_eventhandler(void*, void*, Event ev);
		static void sc_handlemouse(void*, void*, MouseButton ev);
		static void sc_handlekeyboard(void*, void*, Key ev);
		static void sc_handletouch(void*, void*, TouchFinger ev);
	};
}

#endif