#pragma once
#include <standard.h>
#include "EngineBase.h"

#include "C/Color.h"
#include "Datatypes/Rect.h"
#include "Classes/File.h"

namespace Components
{
    class StaticTexture;
}
class DrawingDevice
{
    ENGINE_BASE
    friend class Components::StaticTexture;
public:
    enum DrawingMode
    {
        dm_Stroke,
        dm_Fill
    };

    Vector2f getScreenCenter();
    Vector2i getScreenSize() {SDL_Rect r; SDL_RenderGetViewport(sdl_renderer, &r); return {r.w, r.h};};
    Vector2i getScreenAbsoluteSize() {int x, y; SDL_GetRendererOutputSize(sdl_renderer, &x, &y); return {x, y};};

    void DrawRectangle(const RectF& _Rectangle, const Color4& _Col, DrawingMode _mode=dm_Fill);
    void DrawRectangleAtWorld(const RectF& _Rectangle, const Color4& _Col, DrawingMode _mode=dm_Fill);

    void DrawRotatedRectangle(const RectF& _Rectangle, const double _angle, const Color4& _Col, DrawingMode _mode = dm_Fill);
    void DrawRotatedRectangleAtWorld(const RectF& _Rectangle, const double _angle, const Color4& _Col, DrawingMode _mode = dm_Fill);

    void DrawCircle(const Vector2f& _Pos, const float _Radius, const Color4& _Col, DrawingMode _mode=dm_Fill);

    void DrawDebug(Vector2f pos);

    void DrawTexture(const RectF& _Rectangle, File& _File);
    void DrawFont(const SDL_Rect* _Bounds, File& _FontFile, const char* text, int count, uint8_t alignment);

    bool LoadFileTexture(File& _File);
private:
    bool m_Locked = true;
    SDL_Renderer *sdl_renderer;
    SDL_Texture *sdl_rectTexture;
    
    SDL_Rect m_viewport;

    std::unordered_map<std::string, SDL_Texture*> m_LoadedTextures;
    std::unordered_map<std::string, TTF_Font*> m_LoadedFonts;

    void processViewport();
    void render();
    void renderCurrentWorld();
    void renderCurrentUI();

    inline void m_switchLock() {m_Locked = !m_Locked;}
    void tr();

    unsigned int scale = 0;
};
