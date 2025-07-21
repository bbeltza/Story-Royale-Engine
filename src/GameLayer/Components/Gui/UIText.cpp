#include <standard.h>
#include "System.h"

#include "GuiComponents.h"



Game::GuiComponents::UIText::UIText()
{
    p_flags = RENDER;
}

void Game::GuiComponents::UIText::LoadFontPath(const char* path)
{
    m_file.Load(path);
}

Game::GuiComponents::UIText::~UIText()
{
}



char Game::GuiComponents::UIText::process_text()
{
    char oldchar = 0;

    if (count >= text.length()) count = -1;
    else if (count >= 0)
    {
        auto it = text.begin() + count;
        oldchar = *it;
        text[count] = 0;
        if (!oldchar) count = -1;
    }

    return oldchar;
}