#pragma once
#include <SDL_events.h>
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

	sre::vec2f UV;
	sre::vec2f Delta;
	
	// SDL Metadata
	SDL_TouchID TouchID; SDL_FingerID FingerID;

	int Pressed;
};