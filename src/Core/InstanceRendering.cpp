#include "Engine.h"
#include "ColorFunctions.h"
#include "Components.h"
#include "GuiComponents.h"

SDL_Renderer* target_renderer;

void DrawingDevice::tr()
{
    target_renderer = sdl_renderer;
}

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

void Game::Components::Shape::Render(Entity* _entity)
{
    const RectI render_rect = getRealRect(_entity);
    //Engine->DrawingContext.DrawRectangleAtWorld(render_rect, {255, 0, 0, 255}, DrawingDevice::dm_Stroke);
    if (!(this->flags & FLAG_ShapeFlags::VISIBLE)) return;
    Vector2i pos = _entity->getWorld()->worldToScreenSpace(_entity->x, _entity->y);


    SDL_SetRenderDrawColor(target_renderer, Color.r, Color.b, Color.g, Color.a);


    if (shape == CIRCLE)
    {
        SDL_RenderFillCircle(target_renderer, pos.X + Rect.Position.X, pos.Y + Rect.Position.Y, Rect.Size.X / 2);
    }
    else
    {
        Engine->DrawingContext.DrawRectangleAtWorld(render_rect, Color);
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

void Game::Components::Sprite::Render(Entity* _entity)
{
    if (textures.empty()) return;
    auto frame = std::min(current_frame, textures.size() - 1);
    current_frame = frame;
    File& texture = textures[frame];
    SDL_Rect render_rect;
    if (!Engine->DrawingContext.LoadFileTexture(texture)) return;

    SDL_QueryTexture((SDL_Texture*)texture.GetUserData(), NULL, NULL, (int*)&render_rect + 2, (int*)&render_rect + 3);

    Vector2i pos = _entity->getWorld()->worldToScreenSpace(_entity->x, _entity->y);

    render_rect.w *= Scale.X;
    render_rect.h *= Scale.Y;
    render_rect.x = pos.X + Offset.X;
    render_rect.y = pos.Y + Offset.Y;

    Engine->DrawingContext.DrawTexture(*(RectI*)&render_rect, texture);
}