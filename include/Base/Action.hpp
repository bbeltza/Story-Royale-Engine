#pragma once
#include "Engine.hpp"

#include "Events/Key.hpp"
#include "Events/Mouse.hpp"
#include "Events/Touch.hpp"

#include "Base/Input.hpp"

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

		inline void add(Input::Button button) { return add_impl(static_cast<int>(button), C_MOUSE); }
		inline void add(SDL_Scancode button) { return add_impl(static_cast<int>(button), C_SCANCODE); }
		inline void add(SDL_KeyCode button) { return add_impl(static_cast<int>(button), C_KEYCODE); }
		inline void remove(Input::Button button) { return remove_impl(static_cast<int>(button), C_MOUSE); }
		inline void remove(SDL_Scancode button) { return remove_impl(static_cast<int>(button), C_SCANCODE); }
		inline void remove(SDL_KeyCode button) { return remove_impl(static_cast<int>(button), C_KEYCODE); }

		bool pressed() const { return !(m_frame < 0); }
		bool just_pressed() const { return m_frame == Runtime::CurrentFrame(); }

	private:
		enum Category
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
		static ConnectionHandle sc_mouse;
		static ConnectionHandle sc_keyboard;
		static ConnectionHandle sc_touch;
		static void sc_handlemouse(void*, void*, const MouseButton* ev);
		static void sc_handlekeyboard(void*, void*, const Key* ev);
		static void sc_handletouch(void*, void*, const TouchFinger* ev);
	};
}