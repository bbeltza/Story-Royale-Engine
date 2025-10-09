#include "Engine.hpp"
#include "Game/World.hpp"
#include "Game/Entity.hpp"
#include "Game/GuiObject.hpp"
#include "Game/GuiComponent.hpp"

#include "Game/Components/Shape.hpp"
#include "Game/Components/Sprite.hpp"
#include "Game/GuiComponents/Fill.hpp"
#include "Game/GuiComponents/Stroke.hpp"
#include "Game/GuiComponents/Text.hpp"

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
    if (!this->flags.Has(VisibleFlag)) return;

    //Engine->DrawingContext.DrawRectangleAtWorld(render_rect, {255, 0, 0, 255}, DrawingDevice::dm_Stroke);
    Vector2f pos = _entity->getWorld()->worldToScreenSpace(_entity->Position.X, _entity->Position.Y);

    
    switch (shape)
    {
        case CircleShape:
	    Engine->DrawingContext.DrawCircle(pos, Rect.Size.X / 2, Color);
            break;
        default:
	    {
		RectF render_rect = getRealRect(_entity);
	        Engine->DrawingContext.DrawRectangleAtWorld(render_rect, Color);
	    }
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

void Components::Sprite::Render(Game::Entity* _entity)
{
    if (textures.empty()) return;

    auto frame = ut_min(current_frame, textures.size() - 1);
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
