#pragma once
#include <SDL.hpp>
#include "Datatypes/Vector.hpp"

struct TouchFinger
{
	TouchFinger(const SDL_TouchFingerEvent* ev, int pressed):
		UV(ev->x, ev->y),
		Delta(ev->dx, ev->dy),
		TouchID(ev->touchId),
		FingerID(ev->fingerId),
		Pressed(pressed)
	{}

	Vector2f Delta;
	Vector2f UV;
	
	// SDL Metadata
	SDL_TouchID TouchID; SDL_FingerID FingerID;

	int Pressed;
};