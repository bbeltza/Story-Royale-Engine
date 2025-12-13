#pragma once
#include "Events/Key.hpp"
#include "Events/Mouse.hpp"
#include "Events/Touch.hpp"

#include "Base/Signal.hpp"

namespace Input
{
	// Signals

	extern Signal<const ::Key*> KeyEvent;
	extern Signal<const ::MouseButton*> MouseButton;
	extern Signal<const ::MouseWheel*> MouseWheel;
	extern Signal<const ::MouseMove*> MouseMove;
	extern Signal<const ::TouchFinger*> FingerTouch;
	extern Signal<const ::TouchFinger*> FingerMove;

	// Mouse button enum

	enum Button : Uint8
	{
		mbLeft = SDL_BUTTON_LEFT,
		mbMiddle = SDL_BUTTON_MIDDLE,
		mbRight = SDL_BUTTON_RIGHT,
		mbX1 = SDL_BUTTON_X1,
		mbX2 = SDL_BUTTON_X2,


		mbANY = UINT8_MAX
	};

	// Keyboard functions
	bool KeyPressed(SDL_KeyCode keycode);
	bool KeyPressed(SDL_Scancode scancode);

	// Mouse functions
	bool MouseButtonPressed(Button button = mbANY);
	sre::vec2ut MouseScreenPosition();
	sre::vec2ut MouseWorldPosition();

	// Touch functions
	int GetFingersPressed(SDL_TouchID touchid = -1);
}