#include <standard.h>
#include "System.h"

#include "GuiComponents.h"

#ifdef __GNUC__
#define strcpy_s(dest, n, src) strncpy(dest, src, n)
#define strcat_s(dest, n, src) strncat(dest, src, n)
#endif

Game::GuiComponents::UIText::UIText()
{
    p_flags = RENDER;
}

void Game::GuiComponents::UIText::LoadFontPath(const char* path)
{
    int rescmp;
    if (strlen(path) < 6)
        rescmp = -1;
    else
        rescmp = strncmp(RES_PREFIX, path, 6);

    if (!rescmp)
    {
        unsigned int s = strlen(path + 6) + 5;
        char* REAL_path = new char[s];
        strcpy_s(REAL_path, 5, "res/");
        strcat_s(REAL_path, s, path + 6);


        printf("%s\n", REAL_path);

        m_font = TTF_OpenFont(REAL_path, 12);
        delete[] REAL_path;
    }
    else m_font = TTF_OpenFont(path, 12);
        
    if (!m_font) sys_errorf("SDL_TTF Error : %s", TTF_GetError());

    printf("%s\n", path);
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