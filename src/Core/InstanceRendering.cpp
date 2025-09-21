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

void GuiComponents::Fill::render(Game::GuiContainer* obj)
{
    auto absolute = getAbsolute(obj);

    Engine->DrawingContext.DrawRectangle(*absolute, color, *getModulate(obj), Vector2f::ZERO);
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


    switch (shape)
    {
        case CircleShape:
            Engine->DrawingContext.DrawCircle(pos, Rect.Size.X / 2, Color);
            break;
        default:
            Engine->DrawingContext.DrawRectangleAtWorld(render_rect, Color);
            break;
    }
}

void GuiComponents::Stroke::render(Game::GuiContainer* obj)
{
    for (unsigned int i = 0; i < size; i++)
    {
        RectF absolute = *getAbsolute(obj);
        absolute.Size.X -= i * 2;
        absolute.Size.Y -= i * 2;
        absolute.Position.X += i;
        absolute.Position.Y += i;

        Engine->DrawingContext.DrawRectangle(absolute, color, *getModulate(obj), Vector2f::ZERO, DrawingDevice::dm_Stroke);
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
    Texture& texture = *textures[frame];

    RectF render_rect(
        Game::World::Current()->worldToScreenSpace(_entity->Position.X, _entity->Position.Y),
        texture.GetSize()
    );

    render_rect.Size *= Scale;
    render_rect.Position += Offset;

    Engine->DrawingContext.DrawTexture(texture, render_rect);
}