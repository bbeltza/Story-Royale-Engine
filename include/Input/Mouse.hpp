#pragma once
#include "Input/API"

#include "Datatypes/Vector.hpp"

enum MouseButton
{
	BUTTON_LEFT = SDL_BUTTON_LEFT,
	BUTTON_MIDDLE = SDL_BUTTON_MIDDLE,
	BUTTON_RIGHT = SDL_BUTTON_RIGHT,
	BUTTON_X1 = SDL_BUTTON_X1,
	BUTTON_X2 = SDL_BUTTON_X2
};

struct MousePressInput : public InputObject
{
	inline InputType getType() override const { return InputType::MousePress; }

	MouseButton Button;
	Vector2f Position;
};

struct MouseMotionInput : public InputObject
{
	inline InputType getType() override const { return InputType::MouseMove; }

	MouseButton State;
	Vector2f Position,
			 Delta;
};