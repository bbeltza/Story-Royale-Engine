#ifndef SREGUI_TEXT_HPP
#define SREGUI_TEXT_HPP

#include <GUI/Component.hpp>
#include <Base/Font.hpp>

namespace sreGUI
{
    struct Text: public Component
    {
        void load(const std::string& path);
        
        inline void assign(const char* str) {m_str = str;}
        template <class... T> inline void format(const char* fmt, T&&... args) {
            m_str.resize(snprintf(NULL, 0, fmt, args... ) + 1 );
            sprintf(&m_str[0], fmt, args...);   // This call looks scary, but remember we already reserved enough space to format the string
                                                // So it is garanteed to not crash unless something wrong happens
                                                // And even if it happens then we are happy to catch another bug so...
        }

        inline int length() const { return (int)m_str.size(); }

        int count = -1;
        unsigned int scale = 0;

        sre::col4 color = sre::BLACK;

        sre::Alignment h_alignment = sre::A_LEFT;
        sre::Alignment v_alignment = sre::A_TOP;
    protected:
        void on_render(const sre::rect2Dut& dimensions) override;
    private:
        std::string m_str;
        sre::Font* m_font = NULL;
        static std::unordered_map<std::string, sre::Font> font_map;
    };
}

#endif