#pragma once
#include <SDL_ttf.h>
#include <standard>

#include "Base/Texture.hpp"
#include "datatypes/rect.hpp"
#include "datatypes/color.hpp"

namespace sre
{
    enum Alignment
    {
        A_LEFT,
        A_CENTER,
        A_RIGHT,

        A_TOP = A_LEFT,
        A_BOTTOM = A_RIGHT
    };

    class Font
    {
    public:
        Font(const char *path, int pt = 12);
        ~Font();

        void render(const sre::rect2Dut &bounds, const sre::col4 &color, const char *text, int count, Alignment halignment = A_LEFT, Alignment valignment = A_TOP);
        void render_line(const sre::vec2ut &start, const sre::col4 &color, const char *text, int count, int acc = 0);

        bool preload(const char *desired);
        bool valid() const { return m_font != NULL; }
    private:
        std::unordered_map<int, Texture> textures;
        std::unordered_map<int, sre::vec2i> sizes;

        TTF_Font *m_font = NULL;
        SDL_RWops *m_rwops = NULL;

        friend struct _containers_service;
    };
}