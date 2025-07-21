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
#if !ENTITY_DRAWCENTERDBG
    if (this->m_Components.empty())
#endif // !1
    {
        Engine->DrawingContext.DrawDebug({this->x, this->y});
    }
}

void Game::Components::Shape::Render(Entity* _entity)
{
    const RectF render_rect = getRealRect(_entity);
    //Engine->DrawingContext.DrawRectangleAtWorld(render_rect, {255, 0, 0, 255}, DrawingDevice::dm_Stroke);
    if (!(this->flags & FLAG_ShapeFlags::VISIBLE)) return;
    Vector2f pos = _entity->getWorld()->worldToScreenSpace(_entity->x, _entity->y);


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
    SDL_FRect* r = getParentAbs();
    SDL_Rect ir{r->x, r->y, r->w, r->h};
    Engine->DrawingContext.DrawFont(&ir, this->m_file, text.c_str(), count, Alignment);
}

// 014FF2F0 018EA550 A long time ago, two powerful men ruled the world -1 1
// 014FF2F0 056A7890 A long time ago, two powerful men ruled the world -1 1

void Game::Components::Sprite::Render(Entity* _entity)
{
    if (textures.empty()) return;
    auto frame = std::min(current_frame, textures.size() - 1);
    current_frame = frame;
    File& texture = *textures[frame];
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