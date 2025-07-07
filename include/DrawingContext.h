#pragma once
#include <SDL.hpp>
#include "EngineBase.h"
#include "datatypes/Rect.h"
#include "datatypes/Color.h"

class DrawingDevice
{
    ENGINE_BASE

public:
    template<class _Num> void DrawRectangle(const Rect<_Num>& _Rectangle, const Color4& _Col) {
        SDL_Rect r{
            _Rectangle.Position.X - _Rectangle.Size.Width/2,
            _Rectangle.Position.Y - _Rectangle.Size.Height/2,
            _Rectangle.Size.Width,
            _Rectangle.Size.Height
        };

        SDL_SetRenderDrawColor(sdl_renderer, _Col.r, _Col.g, _Col.b, _Col.a);
        SDL_RenderFillRect(sdl_renderer, &r);
    };

private:
    bool m_Locked = true;
    SDL_Renderer *sdl_renderer;

    void render();
    void renderCurrentWorld();
    void renderCurrentUI();

    inline void m_switchLock() {m_Locked = !m_Locked;}
    void tr();
};