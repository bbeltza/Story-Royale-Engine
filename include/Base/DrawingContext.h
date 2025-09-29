#pragma once
#include <standard.h>
#include <SDL.hpp>
#include "EngineBase.h"

#include "Datatypes/Color.h"
#include "Datatypes/Rect.h"

#include "Classes/File.h"
#include "Classes/Texture.h"

namespace Components
{
    class StaticTexture;
}
class DrawingDevice
{
    ENGINE_BASE
    friend class Components::StaticTexture;
    friend class Texture;

    DrawingDevice(EngineClass*);
    ~DrawingDevice();
    void Setup();
public:
    enum DrawingMode
    {
        dm_Stroke,
        dm_Fill
    };

    Vector2f getScreenCenter();
    Vector2i getScreenSize() {SDL_Rect r; SDL_RenderGetViewport(sdl_renderer, &r); return {r.w, r.h};};
    Vector2i getScreenAbsoluteSize() {int x, y; SDL_GetRendererOutputSize(sdl_renderer, &x, &y); return {x, y};};

    void DrawRectangle(const RectF& Rectangle, const Color4& Color, const Color4& Modulate=Color4::WHITE, const Vector2f &AnchorPoint=Vector2f::CENTER, DrawingMode Mode=dm_Fill);
    void DrawRectangleAtWorld(RectF Rectangle, const Color4& Color, const Color4& Modulate=Color4::WHITE, const Vector2f &AnchorPoint=Vector2f::CENTER, DrawingMode Mode=dm_Fill);

    void DrawRotatedRectangle(const RectF& _Rectangle, const double _angle, const Color4& _Col, DrawingMode _mode = dm_Fill);
    void DrawRotatedRectangleAtWorld(const RectF& _Rectangle, const double _angle, const Color4& _Col, DrawingMode _mode = dm_Fill);

    void DrawCircle(const Vector2f& _Pos, const float _Radius, const Color4& _Col, DrawingMode _mode=dm_Fill);

    void DrawDebug(Vector2f pos);

    void DrawTexture(Texture& _Texture, const RectF& Rectangle, const Color4& Modulate=Color4::WHITE, const Vector2f& AnchorPoint=Vector2f::CENTER);

    void LegacyDrawTexture(const RectF& _Rectangle, File& _File);
    bool LegacyLoadFileTexture(File& _File);
private:
    SDL_Renderer *sdl_renderer;
    SDL_Texture *sdl_rectTexture;
    
    SDL_Rect m_viewport;

    SDL_mutex *m_lockmutex = SDL_CreateMutex();

    std::unordered_map<std::string, SDL_Texture*> m_LoadedTextures;
    std::unordered_map<std::string, TTF_Font*> m_LoadedFonts;

    Texture::Queue textures_toload;

    void processViewport();
    void render();
    void renderCurrentWorld();
    void renderCurrentUI();

    unsigned int scale = 0;
};
