#pragma once
#include <SDL.hpp>
#include <standard.h>

#include "Classes/Texture.h"
#include "Datatypes/Rect.h"

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

    File m_file;
    TTF_Font* m_font;

};