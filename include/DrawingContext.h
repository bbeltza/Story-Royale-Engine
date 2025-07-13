#pragma once
#include <standard.h>
#include "EngineBase.h"
#include "datatypes/Rect.h"
#include "datatypes/Color.h"
#include "datatypes/Vector.h"
#include "File.h"

class DrawingDevice
{
    ENGINE_BASE

public:
    enum DrawingMode
    {
        dm_Stroke,
        dm_Fill
    };

    void getScreenCenter(unsigned int* x, unsigned int* y);

    void DrawRectangle(const RectI& _Rectangle, const Color4& _Col, DrawingMode _mode=dm_Fill);
    void DrawRectangleAtWorld(const Rect<int>& _Rectangle, const Color4& _Col, DrawingMode _mode=dm_Fill);

    void DrawRotatedRectangle(const RectI& _Rectangle, const double _angle, const Color4& _Col);
    void DrawRotatedRectangleAtWorld(const RectI& _Rectangle, const double _angle, const Color4& _Col);

    void DrawDebug(Vector2i pos);

    void DrawTexture(const RectI& _Rectangle, File& _File);
    bool LoadFileTexture(File& _File);
private:
    bool m_Locked = true;
    SDL_Renderer *sdl_renderer;
    SDL_Texture *sdl_rectTexture;
    
    SDL_Rect m_viewport;

    std::unordered_map<std::string, SDL_Texture*> m_LoadedTextures;

    void processViewport();
    void render();
    void renderCurrentWorld();
    void renderCurrentUI();

    inline void m_switchLock() {m_Locked = !m_Locked;}
    void tr();
};
