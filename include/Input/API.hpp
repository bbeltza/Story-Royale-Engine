#pragma once
#include <SDL.hpp>

struct InputObject
{
	enum InputType
	{
		Keyboard,
		MousePress,
		MouseMotion,
		MouseWheel,
		Gamepad,
		Joystick,
		Touch
	};
	virtual InputType getType() const = 0;

	InputObject(int pressed): Pressed(pressed) {}
	int Pressed;
};