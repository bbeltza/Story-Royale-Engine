#include <standard.h>

#include "Game/GuiComponents/Text.h"

std::unordered_map<std::string, File> GuiComponents::Text::file_map;

GuiComponents::Text::Text()
{
    p_flags = RENDER;
}

void GuiComponents::Text::LoadFont(const char* path)
{
    file_map[path].Load(path);
    m_path = path;
}

GuiComponents::Text::~Text()
{
}