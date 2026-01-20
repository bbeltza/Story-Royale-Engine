#include <GUI/Components/text.hpp>

using namespace sreGUI;

std::unordered_map<std::string, sre::Font> Text::font_map;

void Text::load(const std::string& path)
{
    if (!font_map.count(path)) font_map.emplace(path, path.c_str());
    
    m_font = &font_map.at(path);
}

#define FONTCHECK if (!m_font) return;

void Text::on_render(const sre::rect2Dut& dimensions)
{
    FONTCHECK

    m_font->preload(m_str.c_str());
    m_font->render(dimensions, color, m_str.c_str(), count, h_alignment, v_alignment);
}