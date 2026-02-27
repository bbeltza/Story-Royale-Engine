#include <ECS/Components/Shape.hpp>
#include <ECS/Scene.hpp>

#include <Core/Draw.hpp>

#include <SDL_shape.h>

using namespace sreECS;

Shape::Shape() : shape(S_RECTANGLE),
				 Component(F_VISIBLE)
{
}

Shape::~Shape()
{
}

bool Shape::in_screen_point(Entity &entity, sre::vec2ut pt) const
{
	const auto w = entity.get_parent();

	switch (shape)
	{
	case S_RECTANGLE:
	{
		sre::rect2Dut _realrect = real_rect(entity);
		return _realrect.intersects(w->camera.toWorldSpace(pt));
	}
	case S_CIRCLE:
	{
		sre::vec2ut screenSpace = w->camera.toScreenSpace(entity.position + rect.position);
		return (screenSpace - pt).magnitude() <= rect.size.x / 2;
	}
	default:
		return false;
	}
}

void Shape::on_render(Entity &entity)
{
	if (!this->flags.has(F_VISIBLE))
		return;

	const auto world = entity.get_parent();

	switch (shape)
	{
	case S_CIRCLE:
		sre_draw(SRE_DRAW_CIRCLE, NULL);
		break;
	default:
	{
		sre::rect2Dut render_rect = real_rect(entity);
		sre::draw(sre::DDRect{
			SRE_DRAWFLAGS_USECAM,
			color,
			render_rect,
			sre::vec2ut::CENTER
		});
		break;
	}
	}
}

bool Shape::on_query(Entity& entity, sre::vec2ut screen_coords) const
{
    return in_screen_point(entity, screen_coords);
}


void Shape::on_pupdate(Entity& entity)
{    
    if (!flags.has(F_CANCOLLIDE))
        return;

    if (flags.has(F_ANCHORED)) return;

    for (Entity& ent : *entity.get_parent())
    {
        if (&ent == &entity) continue;

        for (Component& comp : ent)
        {
			auto compshape = dynamic_cast<Shape*>(&comp);
			if (compshape == NULL) continue;
			if (!compshape->enabled()) continue;
            if (!compshape->flags.has(F_CANCOLLIDE)) continue;

            sre::rect2Dut thisRect = real_rect(entity),
                          otherRect = compshape->real_rect(ent);

            if (compshape->collides(ent, thisRect))
            {
                sre::vec2ut distance = (thisRect.position - otherRect.position);
                sre::vec2ut radius = (otherRect.size/2 + thisRect.size/2);

                sre::vec2ut sign = {distance.x < 0 ? -1.0_ut : 1, distance.y < 0 ? -1 : 1.0_ut};
                sre::vec2ut pushdist = radius * sign - distance;

                if (!compshape->flags.has(F_ANCHORED)) pushdist = pushdist / 2;

                sre::unit absdx = abs(pushdist.x), absdy = abs(pushdist.y);

                if (absdx < absdy) entity.position.x += pushdist.x;
                else if (absdx > absdy) entity.position.y += pushdist.y;
                else entity.position += pushdist;
            }
        }
    }
}
