#include "Game/Components/Shape.hpp"

#include "Game/Entity.hpp"
#include "Game/World.hpp"

#include "Base/Display.hpp"

#include <SDL_shape.h>

Components::Shape::CollisionSet* Components::Shape::collider_set = nullptr;

Components::Shape::Shape() :
	Rect(),
	shape(RectangleShape),
	flags(VisibleFlag)
{
	if (!collider_set)
		collider_set = new CollisionSet;

	if (flags.Has(CanCollideFlag))
		collider_set->insert(this);
}

Components::Shape::~Shape()
{
	if (!collider_set) return;

	collider_set->erase(this);
	if (collider_set->empty())
	{
		delete collider_set;
		collider_set = nullptr;
	}
}

bool Components::Shape::isInScreenPoint(Game::Entity* p, Vector2f pt)
{
	auto w = p->getWorld<Game::World>();

	static SDL_FPoint fpt;
	static SDL_FRect r;
	fpt.x = pt.X;
	fpt.y = pt.Y;

	static Vector2f screenSpace;

	switch (shape)
	{
	case RectangleShape:
		screenSpace = w->worldToScreenSpace(p->Position.X + Rect.getLeft(), p->Position.Y + Rect.getTop());
		r.x = screenSpace.X;
		r.y = screenSpace.Y;
		r.w = Rect.Size.X;
		r.h = Rect.Size.Y;
		return SDL_PointInFRect(&fpt, &r);
	case CircleShape:
		screenSpace = w->worldToScreenSpace(p->Position.X + Rect.Position.X, p->Position.Y + Rect.Position.Y);
		return (screenSpace - pt).getMagnitude() <= Rect.Size.X / 2;
	default:
		return 0;
		break;
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
		Display::DrawCircle(pos, Rect.Size.X / 2, Color);
		break;
	default:
	{
		RectF render_rect = getRealRect(_entity);
		Display::DrawRectangleAtWorld(render_rect, Color);
		break;
	}
	}
}
