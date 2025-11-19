#pragma once
#include <SDL_ttf.h>
#include <standard>

#include "Base/Texture.hpp"
#include "Datatypes/Rect.hpp"

class Font
{
    public:
    enum HAlign
    {
        HLeft,
        HCenter,
        HRight
    };
    enum VAlign
    {
        VTop,
        VCenter,
        VBottom
    };
    public:
    Font(const char* path, int pt=12);
    ~Font();

    void Render(const RectF& bounds, const char* text, int count, HAlign halignment=HLeft, VAlign valignment=VTop);
    void RenderLine(const Vector2f& start, const char* text, int count, int acc);
    bool PreloadTextures(const char* desired);
    private:
    std::unordered_map<char, Texture> textures;
    std::unordered_map<char, Vector2i> sizes;

    TTF_Font* m_font;
    SDL_RWops* m_src;

    friend struct _containers_service;
};