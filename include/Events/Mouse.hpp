#pragma once
#include <SDL_events.h>
#include "datatypes/vector.hpp"

struct MouseButton
{
    MouseButton(const SDL_MouseButtonEvent* ev, float ratio):
        pressed(ev->state),
        id(ev->which),
        button(ev->button),
        clicks(ev->clicks),
        position(ev->x / ratio, ev->y / ratio)
    {}

    const bool pressed;
    const Uint32 id;
    const Uint8 button;
    const Uint8 clicks;

    const sre::vec2ut position;
};

struct MouseMove
{
    MouseMove(const SDL_MouseMotionEvent* ev, float ratio) :
        id(ev->which),
        state(ev->state),
        position(ev->x / ratio, ev->y / ratio),
        delta(ev->xrel / ratio, ev->yrel / ratio)
    { }

    const Uint32 id;
    const Uint32 state;

    const sre::vec2ut position;
    const sre::vec2ut delta;
};

struct MouseWheel
{
    MouseWheel(const SDL_MouseWheelEvent* ev, float ratio):
        id(ev->which),
        amount(ev->x, ev->y),
        precise_amount(ev->preciseX, ev->preciseY),
        position(ev->mouseX / ratio, ev->mouseY / ratio)
    {}

    const Uint32 id;

    const sre::vec2i amount;
    const sre::vec2f precise_amount;
    const sre::vec2f position;
};

#undef ratio