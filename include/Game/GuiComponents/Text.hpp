#pragma once
#include "Game/GuiComponent.hpp"
#include "Base/Font.hpp"

namespace GuiComponents
{
    class Text: public ::Game::GuiComponent
    {
    public:
        Text();
        ~Text();

        void LoadFont(std::string path);
        
        inline void assign(const char* str) {m_str = str;}
        template <class... T> inline void format(const char* fmt, T&&... args) {
            m_str.resize(snprintf(NULL, 0, fmt, args... ) + 1 );
            sprintf(&m_str[0], fmt, args...);   // This call looks scary, but remember we already reserved enough space to format the string
                                                // So it is garanteed to not crash unless something wrong happens
                                                // And even if it happens then we are happy to catch another bug so...
        }

        inline int getLength() { return (int)m_str.size(); }

        int count = -1;
        unsigned int scale = 0;

        sre::col3 color = sre::col3::BLACK;

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
