#pragma once
#include "Game/GuiComponent.h"
#include "Classes/Font.h"
#include "SDL.hpp"
#include <fmt/format.h>

namespace GuiComponents
{
    class Text: public ::Game::GuiComponent
    {
    public:
        Text();
        ~Text();

        void LoadFont(std::string path);
        
        inline void assign(const char* str) {m_str = str;}
        template <class... T> inline void format(const char* fmt, T&&... args) { m_str = fmt::format(fmt, args...); }

        inline int getLength() { return m_str.size(); }

        int count = -1;
        unsigned int scale = 0;

        Color3 color = { 0, 0, 0 };

        Font::HAlign h_alignment = Font::HLeft;
        Font::VAlign v_alignment = Font::VTop;
    protected:
        void render(::Game::GuiContainer*) override;
        void process_position(::Game::GuiContainer*) override;
    private:
        std::string m_str;
        Font* m_font = nullptr;
        static std::unordered_map<std::string, Font> font_map;
    };
}
