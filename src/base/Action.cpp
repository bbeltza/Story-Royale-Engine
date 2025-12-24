#include "Base/Action.hpp"
#include "Engine.hpp"

Action::list* Action::s_actions = nullptr;
ConnectionHandle Action::sc_mouse;
ConnectionHandle Action::sc_keyboard;
ConnectionHandle Action::sc_touch;

extern "C" void __init_actions()
{
	Action::sc_mouse = Input::MouseButton.Connect(Action::sc_mouseHandle, NULL);
	Action::sc_keyboard = Input::KeyEvent.Connect(Action::sc_keyboardHandle, NULL);
	Action::sc_touch = Input::FingerTouch.Connect(Action::sc_touchHandle, NULL);
}

void Action::push_self()
{
	if (!s_actions)
	{
		s_actions = new list;
	}
	s_actions->push_back(this);
}

Action::~Action()
{
	s_actions->remove(this);
	if (s_actions->empty())
	{
		delete s_actions;
		s_actions = nullptr;
	}
}

#define sa_check if (!s_actions) return;
#define act_frame act->press_frame = Runtime::CurrentFrame()

void Action::sc_mouseHandle(void*, void*, const MouseButton* ev)
{
	sa_check

	for (auto act : *s_actions)
	{
		Input::Button button = (Input::Button)ev->button;
		if (act->m_mousebuttons.count(button))
		{
			act->m_mousebuttons.at(button) = ev->pressed;
			if (ev->pressed)
				act_frame;
		}
	}
}

void Action::sc_keyboardHandle(void*, void*, const Key* ev)
{
	sa_check
	if (ev->repeat) return;

	for (auto act : *s_actions)
	{
		if (act->m_scancodes.count(ev->scanCode))
		{
			act->m_scancodes.at(ev->scanCode) = ev->pressed;
			goto press_sct;
		}
		if (act->m_keycodes.count(ev->keyCode))
		{
			act->m_keycodes.at(ev->keyCode) = ev->pressed;
			goto press_sct;
		}

		continue;
		press_sct:
		if (!ev->repeat && ev->pressed)
			act_frame;
	}
}

void Action::sc_touchHandle(void*, void*, const TouchFinger* ev)
{
	sa_check
	if (ev->Pressed) return;

	for (auto act : *s_actions)
	{
		if (act->enable_touch)
			act_frame;
	}
}