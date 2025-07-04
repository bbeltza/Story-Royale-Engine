#include "Engine.h"
#include "ColorFunctions.h"
#include "Components.h"
#include "GuiComponents.h"

static SDL_Renderer* target_renderer;

void WindowClass::target_render() { target_renderer = this->sdl_renderer; }

void Game::GuiObject::_render()
{
    SDL_SetRenderDrawColorMod(target_renderer, &color, &p_modulate);
    SDL_RenderFillRectF(target_renderer, &p_absolute);
}

void Game::Entity::_debugDraw()
{
    Vector2i rPt = m_world->worldToScreenSpace(this->x, this->y);
#if !ENTITY_DRAWCENTERDBG
    if (this->m_Components.empty())
#endif // !1
    {
        SDL_SetRenderDrawColor(target_renderer, 255, 64, 0, 255);
        SDL_RenderDrawLineF(target_renderer, rPt.X - ENTITY_DBGLINESIZE, rPt.Y, rPt.X + ENTITY_DBGLINESIZE, rPt.Y);
        SDL_RenderDrawLineF(target_renderer, rPt.X, rPt.Y - ENTITY_DBGLINESIZE, rPt.X, rPt.Y + ENTITY_DBGLINESIZE);
    }
}

void Game::Components::Shape::render(int x, int y)
{
    if (!(this->flags & FLAG_ShapeFlags::VISIBLE)) return;

    SDL_SetRenderDrawColor(target_renderer, Color.r, Color.b, Color.g, Color.a);

    const float cx = x + xOffset;
    const float cy = y + yOffset;

    if (shape == CIRCLE)
    {
        SDL_RenderFillCircle(target_renderer, cx, cy, Width / 2);
    }
    else
    {
        m_renderRect.w = Width;
        m_renderRect.h = Height;
        m_renderRect.x = cx - m_renderRect.w / 2;
        m_renderRect.y = cy - m_renderRect.h / 2;

        SDL_RenderFillRectF(target_renderer, &m_renderRect);
    }
}
void Game::GuiComponents::UIStroke::render()
{
    SDL_SetRenderDrawColorMod(target_renderer, &color, getParentMod());

    for (int i = 0; i < size; i++)
    {
        SDL_FRect r = *getParentAbs();
        r.h -= i * 2;
        r.w -= i * 2;
        r.x += i;
        r.y += i;

        SDL_RenderDrawRectF(target_renderer, &r);
    }
}
void Game::GuiComponents::UIText::render()
{
    if (count == 0) return;

    char oldchar = process_text();
    const char* c_str = text.c_str();

    unsigned int s = scale + 1;
    int w, h;

    SDL_FRect* absolute = getParentAbs();

    if (!m_cacheTexture || m_cache_Abs.w != absolute->w || m_cacheText != text)
    {
        if (m_cacheTexture)
            SDL_DestroyTexture(m_cacheTexture);
        m_cache_Abs = *absolute;
        m_cacheText = text;

        SDL_Surface* textSurface = TTF_RenderUTF8_Solid_Wrapped(m_font, text.c_str(), { color.r, color.g, color.b }, absolute->w / s);
        m_cacheTexture = SDL_CreateTextureFromSurface(target_renderer, textSurface);

        SDL_FreeSurface(textSurface);
    }

    SDL_QueryTexture(m_cacheTexture, NULL, NULL, &w, &h);
    SDL_Rect iAbs = { absolute->x, absolute->y, w * s, h * s };


    //printf("%d, %d | %f, %f\n", w, h, getParentAbs()->w, getParentAbs()->h);

    SDL_RenderCopy(target_renderer, m_cacheTexture, NULL, &iAbs);

    //*absolute = oldAbs;
    if (oldchar) text[count] = oldchar;
}