#pragma once
#include <SDL.hpp>
#include "Datatypes/Vector.hpp"

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

    const Vector2f position;
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

    const Vector2f position;
    const Vector2f delta;
};

struct MouseWheel
{
    MouseWheel(const SDL_MouseWheelEvent* ev, float ratio):
        id(ev->which),
        amount(ev->x, ev->y),
        p_amount(ev->preciseX, ev->preciseY),
        position(ev->mouseX / ratio, ev->mouseY / ratio)
    {}

    const Uint32 id;

    const Vector2f p_amount;
    const Vector2i amount;
    const Vector2f position;
};

#undef ratio