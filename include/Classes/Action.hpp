#pragma once
#include "Engine.hpp"

#include "Events/Key.hpp"
#include "Events/Mouse.hpp"
#include "Events/Touch.hpp"

#include "Base/Input.hpp"

extern "C" void __init_actions();

class Action
{
	friend void __init_actions();
	typedef std::list<Action*> list;
	static list* s_actions;
	static ConnectionHandle sc_mouse;
	static ConnectionHandle sc_keyboard;
	static ConnectionHandle sc_touch;
	static void sc_mouseHandle(void*, void*, const MouseButton* ev);
	static void sc_keyboardHandle(void*, void*, const Key* ev);
	static void sc_touchHandle(void*, void*, const TouchFinger* ev);

public:
	Action(const Action& other) : press_frame(-1), m_keycodes(other.m_keycodes), m_scancodes(other.m_scancodes), m_mousebuttons(other.m_mousebuttons), enable_touch(other.enable_touch) { push_self(); }
	~Action();

	template <typename... _Args>
	Action(bool touch, _Args... args) : enable_touch(touch) { _addinputs_recurse(args...); push_self(); }
	template <typename... _Args>
	Action(_Args... args) : Action(false, args...) {}

	inline void AddInput(SDL_KeyCode keyCode) { m_keycodes[keyCode] = false; }
	inline void AddInput(SDL_Scancode scanCode) { m_scancodes[scanCode] = false; }
	inline void AddInput(Input::Button mouseButton) { m_mousebuttons[mouseButton] = false; }

	inline bool isPressed() const {
		return (isKeyCodePressed() || isScanCodePressed() || isMousePressed() || isTouchPressed());
	}
	inline bool isJustPressed() const {
		return press_frame == Runtime::CurrentFrame();
	}

private:
	long long press_frame = -1;
	bool enable_touch = false;

	std::unordered_map<SDL_KeyCode, bool> m_keycodes;
	std::unordered_map<SDL_Scancode, bool> m_scancodes;
	std::unordered_map<Input::Button, bool> m_mousebuttons;

	inline bool isTouchPressed() const { if (!enable_touch) return false; return Input::GetFingersPressed() != 0; }
	inline bool isKeyCodePressed() const { for (auto& kv : m_keycodes) if (kv.second) return true; return false; }
	inline bool isScanCodePressed() const { for (auto& kv: m_scancodes) if (kv.second) return true; return false; }
	inline bool isMousePressed() const { for (auto& kv: m_mousebuttons) if (kv.second) return true; return false; }

	template <typename First, typename... Rest>
	inline void _addinputs_recurse(First first, Rest... args) {
		AddInput(first);
		_addinputs_recurse(args...);
	}
	inline void _addinputs_recurse() {}

	void push_self();
};