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
    SDL_RWops* temp_rw = SDL_RWFromConstMem(m_file.getRawData(), m_file.getSize());
    m_font = TTF_OpenFontRW(temp_rw, 1, 12);
}

Game::GuiComponents::UIText::~UIText()
{
    if (m_font)
        TTF_CloseFont(m_font);
    if (m_cacheTexture)
        SDL_DestroyTexture(m_cacheTexture);
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