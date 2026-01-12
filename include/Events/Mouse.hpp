#pragma once
#include <SDL_events.h>
#include "Datatypes/Vector.hpp"

struct MouseButton
{
    MouseButton() = default;
    MouseButton(const SDL_MouseButtonEvent* ev, float ratio):
        pressed(ev->state),
        id(ev->which),
        button(ev->button),
        clicks(ev->clicks),
        position(ev->x / ratio, ev->y / ratio)
    {}
    
    bool pressed;
    Uint32 id;
    Uint8 button;
    Uint8 clicks;

    sre::vec2ut position;
};

struct MouseMove
{
    MouseMove() = default;
    MouseMove(const SDL_MouseMotionEvent* ev, float ratio) :
        id(ev->which),
        state(ev->state),
        position(ev->x / ratio, ev->y / ratio),
        delta(ev->xrel / ratio, ev->yrel / ratio)
    { }

    Uint32 id;
    Uint32 state;

    sre::vec2ut position;
    sre::vec2ut delta;
};

struct MouseWheel
{
    MouseWheel() = default;
    MouseWheel(const SDL_MouseWheelEvent* ev, float ratio):
        id(ev->which),
        amount(ev->x, ev->y),
        precise_amount(ev->preciseX, ev->preciseY),
        position(ev->mouseX / ratio, ev->mouseY / ratio)
    {}

    Uint32 id;

    sre::vec2i amount;
    sre::vec2f precise_amount;
    sre::vec2f position;
};

#undef ratio