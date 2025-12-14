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

bool Components::Shape::isInScreenPoint(Game::Entity* p, sre::vec2ut pt)
{
	auto w = p->getWorld<Game::World>();

	static SDL_FPoint fpt;
	static SDL_FRect r;
	fpt.x = pt.x;
	fpt.y = pt.y;

	sre::vec2ut screenSpace;

	switch (shape)
	{
	case RectangleShape:
		screenSpace = w->worldToScreenSpace(p->Position.x + Rect.left(), p->Position.y + Rect.top());
		r.x = screenSpace.x;
		r.y = screenSpace.y;
		r.w = static_cast<float>(Rect.size.x);
		r.h = static_cast<float>(Rect.size.y);
		return SDL_PointInFRect(&fpt, &r);
	case CircleShape:
		screenSpace = w->worldToScreenSpace(p->Position.x + Rect.position.x, p->Position.y + Rect.position.y);
		return (screenSpace - pt).magnitude() <= Rect.size.x / 2;
	default:
		return false;
	}
}

void Components::Shape::Render(Game::Entity* _entity)
{
	if (!this->flags.has(VisibleFlag)) return;

	const Game::World* world = _entity->getWorld();

	switch (shape)
	{
	case CircleShape:
		Display::DrawCircle(_entity->Position, Rect.size.x / 2, Color, Display::dm_Fill, world);
		break;
	default:
	{
		sre::rect2Dut render_rect = getRealRect(_entity);
		Display::DrawRectangle(render_rect, Color, sre::vec2f::CENTER, Display::dm_Fill, world);
		break;
	}
	}
}
