#pragma once
#include "EngineBase.h"
#include "datatypes/Rect.h"
#include "datatypes/Color.h"
#include "datatypes/Vector.h"

class DrawingDevice
{
    ENGINE_BASE

public:
    void getScreenCenter(unsigned int* x, unsigned int* y);

    void DrawRectangle(const RectI& _Rectangle, const Color4& _Col);
    void DrawRectangleAtWorld(const Rect<int>& _Rectangle, const Color4& _Col);

    void DrawRotatedRectangle(const RectI& _Rectangle, const double _angle, const Color4& _Col);
    void DrawRotatedRectangleAtWorld(const RectI& _Rectangle, const double _angle, const Color4& _Col);

    void DrawDebug(Vector2i pos);
private:
    bool m_Locked = true;
    SDL_Renderer *sdl_renderer;
    SDL_Texture *sdl_rectTexture;
    
    SDL_Rect m_viewport;

    void processViewport();
    void render();
    void renderCurrentWorld();
    void renderCurrentUI();

    inline void m_switchLock() {m_Locked = !m_Locked;}
    void tr();
};
