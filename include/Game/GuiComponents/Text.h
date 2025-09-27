#pragma once
#include "Game/GuiComponent.h"
#include "Classes/File.h"
#include "SDL.hpp"
#include <fmt/format.h>

namespace GuiComponents
{
    class Text: public ::Game::GuiComponent
    {
    public:
        enum AlignMode
            {
                AlCentered = TTF_WRAPPED_ALIGN_CENTER,
                AlLeft = TTF_WRAPPED_ALIGN_LEFT,
                AlRight = TTF_WRAPPED_ALIGN_RIGHT
            } Alignment;
    public:
        Text();
        ~Text();

        void LoadFont(const char* path);
        
        inline void assign(const char* str) {m_str = str;}
        template <class... T> inline void format(const char* fmt, T&&... args) { m_str = fmt::format(fmt, args...); }

        inline int getLength() { return m_str.size(); }

        int count = -1;
        unsigned int scale = 0;

        Color3 color = { 0, 0, 0 };
    protected:
        void render(::Game::GuiContainer*) override;
    private:
        const char* m_path;
        std::string m_str;
        static std::unordered_map<std::string, File> file_map;
    };
}
