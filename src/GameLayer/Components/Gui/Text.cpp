#include <standard.h>

#include "Game/GuiComponents/Text.h"

GuiComponents::Text::Text()
{
    p_flags = RENDER;
}

void GuiComponents::Text::LoadFont(const char* path)
{
    m_file.Load(path);
}

GuiComponents::Text::~Text()
{
}