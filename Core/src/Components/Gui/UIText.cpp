#include <standard.h>
#include "System.h"

#include "GuiComponents.h"

Game::GuiComponents::UIText::UIText()
{
    p_flags = RENDER;

    int rescmp;
    if (strlen(font) < 6)
        rescmp = -1;
    else
        rescmp = strncmp(RES_PREFIX, font, 6);

    if (!rescmp)
    {
        unsigned int s = strlen(font + 6) + 5;
        char* REAL_path = new char[s];
        strcpy_s(REAL_path, 5, "res/");
        strcat_s(REAL_path, s, font + 6);

        printf("%s\n", REAL_path);

        m_font = TTF_OpenFont(REAL_path, 12);
        delete[] REAL_path;
    }
    else m_font = TTF_OpenFont(font, 12);
        
    if (!m_font) sys_errorf("SDL_TTF Error : %s", TTF_GetError());


    printf("%s\n", font);
}

Game::GuiComponents::UIText::~UIText()
{
    if (m_font)
        TTF_CloseFont(m_font);
}



void Game::GuiComponents::UIText::render()
{
    SDL_FRect* absolute = getParentAbs();
    unsigned int s = scale + 1;

    SDL_Surface* textSurface = TTF_RenderUTF8_Solid_Wrapped(m_font, text, {color.r, color.g, color.b}, absolute->w/s);
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(Engine::Window::sdl_Renderer, textSurface);

    int w, h;

    SDL_QueryTexture(textTexture, NULL, NULL, &w, &h);
    absolute->w = w * s;
    absolute->h = h * s;
    SDL_Rect iAbs = { absolute->x, absolute->y, absolute->w, absolute->h };


    printf("%d, %d | %f, %f\n", w, h, getParentAbs()->w, getParentAbs()->h);

    putchar(8);

    SDL_RenderCopy(Engine::Window::sdl_Renderer, textTexture, NULL, &iAbs);

    SDL_DestroyTexture(textTexture);
    SDL_FreeSurface(textSurface);
}