#pragma once
#include <SDL.hpp>
#include "Vector.h"

struct MouseButton
{
    MouseButton(const SDL_MouseButtonEvent* ev):
        pressed(ev->state),
        id(ev->which),
        button(ev->button),
        clicks(ev->clicks),
        position(ev->x, ev->y)
    {}

    const bool pressed;
    const Uint32 id;
    const Uint8 button;
    const Uint8 clicks;

    const Vector2i position;
};

struct MouseMove
{
    MouseMove(const SDL_MouseMotionEvent* ev) :
        id(ev->which),
        state(ev->state),
        position(ev->x, ev->y),
        delta(ev->xrel, ev->yrel)
    { }

    const Uint32 id;
    const Uint32 state;

    const Vector2i position;
    const Vector2i delta;
};

struct MouseWheel
{
    MouseWheel(const SDL_MouseWheelEvent* ev):
        id(ev->which),
        amount(ev->x, ev->y),
        p_amount(ev->preciseX, ev->preciseY),
        position(ev->mouseX, ev->mouseY)
    {}

    const Uint32 id;

    const Vector2f p_amount;
    const Vector2i amount;
    const Vector2i position;
};