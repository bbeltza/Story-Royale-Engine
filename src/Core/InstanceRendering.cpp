#include "Engine.h"
#include "Game/World.h"
#include "Game/Entity.h"
#include "Game/GuiObject.h"
#include "Game/GuiComponent.h"

#include "Game/Components/Shape.h"
#include "Game/Components/Sprite.h"
#include "Game/GuiComponents/Fill.h"
#include "Game/GuiComponents/Stroke.h"
#include "Game/GuiComponents/Text.h"

#include "config.h"

SDL_Renderer* target_renderer;

void DrawingDevice::tr()
{
    target_renderer = sdl_renderer;
}

void GuiComponents::Fill::render(Game::GuiContainer* obj)
{
    auto absolute = getAbsolute(obj);
    SDL_FRect r = *(SDL_FRect*)absolute;

    SDL_SetRenderDrawColorMod(target_renderer, reinterpret_cast<SDL_Color*>(&color), reinterpret_cast<SDL_Color*>(getModulate(obj)));
    SDL_RenderFillRectF(target_renderer, &r);
}

void Game::Entity::_debugDraw()
{
#if DRAW_ENT_CENTER_ONLY_EMPTY
    if (this->m_Components.empty())
#endif
    {
        Engine->DrawingContext.DrawDebug(Position);
    }
}

void Components::Shape::Render(Game::Entity* _entity)
{
    if (!(this->flags.Has(VisibleFlag))) return;

    const RectF render_rect = getRealRect(_entity);
    //Engine->DrawingContext.DrawRectangleAtWorld(render_rect, {255, 0, 0, 255}, DrawingDevice::dm_Stroke);
    Vector2f pos = _entity->getWorld<Game::World>()->worldToScreenSpace(_entity->Position.X, _entity->Position.Y);


    
    if (shape == CircleShape)
    {
        Engine->DrawingContext.DrawCircle(pos, Rect.Size.X / 2, Color);
    }
    else
    {
        Engine->DrawingContext.DrawRectangleAtWorld(render_rect, Color);
    }
}

void GuiComponents::Stroke::render(Game::GuiContainer* obj)
{
    SDL_SetRenderDrawColorMod(target_renderer, reinterpret_cast<SDL_Color*>(&color), reinterpret_cast<SDL_Color*>(getModulate(obj)));

    RectF* absolute = getAbsolute(obj);
    for (unsigned int i = 0; i < size; i++)
    {
        SDL_FRect r = *(SDL_FRect*)absolute;
        r.h -= i * 2;
        r.w -= i * 2;
        r.x += i;
        r.y += i;

        SDL_RenderDrawRectF(target_renderer, &r);
    }
}
void GuiComponents::Text::render(Game::GuiContainer* obj)
{
    RectF* r = getAbsolute(obj);
    SDL_Rect ir{(int)r->Position.X, (int)r->Position.Y, (int)r->Size.X, (int)r->Size.Y};
    Engine->DrawingContext.DrawFont(&ir, this->m_file, m_str.c_str(), count, Alignment);
}

void Components::Sprite::Render(Game::Entity* _entity)
{
    if (textures.empty()) return;
    auto frame = std::min(current_frame, textures.size() - 1);
    current_frame = frame;
    File& texture = *textures[frame];
    if (!Engine->DrawingContext.LoadFileTexture(texture)) return;
    
    int w, h;
    SDL_QueryTexture((SDL_Texture*)texture.GetUserData(), NULL, NULL, &w, &h);

    RectF render_rect(_entity->getWorld<Game::World>()->worldToScreenSpace(_entity->Position.X, _entity->Position.Y), {w, h});

    render_rect.Size *= Scale;
    render_rect.Position += Offset;

    Engine->DrawingContext.DrawTexture(render_rect, texture);
}