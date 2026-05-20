#ifndef SREGUI_TEXT_HPP
#define SREGUI_TEXT_HPP

#include <GUI/Component.hpp>
#include <Base/Font.hpp>

namespace sreGUI
{
    struct Text: public Component
    {
        Text() = default;
        Text(const sre::Font& font, const char* text=""): m_fonthandle(font), m_str(text) {}

        void set_font(const sre::Font& font) { m_fonthandle = font; }

        inline void assign(const char* str) {m_str = str;}
        template <class... T> inline void format(const char* fmt, T... args) {
            m_str.resize(snprintf(NULL, 0, fmt, args... ) + 1 );
            sprintf(&m_str[0], fmt, args...);   // This call looks scary, but remember we already reserved enough space to format the string
                                                // So it is garanteed to not crash unless something wrong happens
                                                // And even if it happens then we are happy to catch another bug so...
        }

        inline int length() const { return (int)m_str.size(); }

        int count = -1;
        int scale = 0;

        sre::col4 color = sre::BLACK;

        sre::alignment h_alignment = sre::ALIGN_LEFT;
        sre::alignment v_alignment = sre::ALIGN_TOP;

        void* modifier_pointer = NULL;
        sre::fontRenderCallback modifier_callback = NULL;
    protected:
        void on_render(const sre::rect2Dut& dimensions) override;
    private:
        std::string m_str;
        sre::Font m_fonthandle;
    };
}

#endif