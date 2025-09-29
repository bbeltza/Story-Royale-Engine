#include <standard.h>

#include "Game/GuiComponents/Text.h"

std::unordered_map<std::string, Font> GuiComponents::Text::font_map;

GuiComponents::Text::Text()
{
    p_flags = RENDER | PROCESS_POSITION;
}

GuiComponents::Text::~Text()
{
}

void GuiComponents::Text::LoadFont(std::string path)
{
    if (!font_map.count(path)) font_map.emplace(path, path.c_str());
    
    m_font = &font_map.at(path);
}

#define FONTCHECK if (!m_font) return;

void GuiComponents::Text::process_position(::Game::GuiContainer* cont)
{
    FONTCHECK

    m_font->PreloadTextures(m_str.c_str());
}

void GuiComponents::Text::render(Game::GuiContainer* obj)
{
    FONTCHECK

    RectF* r = getAbsolute(obj);

    //m_font->RenderLine(r->Position, m_str.c_str(), count);
    m_font->Render(*r, m_str.c_str(), count, h_alignment, v_alignment);
}