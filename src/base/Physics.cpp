#include <standard>

#include "Game/World.hpp"
#include "Game/Entity.hpp"
#include "Game/Component.hpp"
#include "Game/Components/Shape.hpp"

using namespace Components;

void Shape::pUpdate(::Game::Entity* entity, TimeStamp dt)
{    
    if (!collider_set) return;
    if (!flags.Has(CanCollideFlag))
    {
        collider_set->erase(this);
        return;
    }
    collider_set->insert(this);

    if (flags.Has(AnchoredFlag)) return;

    for (auto ent : entity->getWorld()->getEntities<Game::Entity>())
    {
        if (ent == entity) continue;

        for (auto comp : ent->getComponents<Shape>())
        {
            if (collider_set->count(comp) == 0) continue;

            RectUt thisRect = getRealRect(entity),
                 otherRect = comp->getRealRect(ent);

            if (comp->collidesWith(ent, thisRect))
            {
                Vector2ut distance = (thisRect.Position - otherRect.Position);
                Vector2ut radius = (otherRect.Size/2 + thisRect.Size/2);

                Vector2ut sign = {distance.X < 0 ? -1 : 1, distance.Y < 0 ? -1 : 1};
                Vector2ut pushdist = radius * sign - distance;

                if (!comp->flags.Has(AnchoredFlag)) pushdist = pushdist / 2;

                Unit absdx = abs(pushdist.X), absdy = abs(pushdist.Y);

                if (absdx < absdy) entity->Position.X += pushdist.X;
                else if (absdx > absdy) entity->Position.Y += pushdist.Y;
                else entity->Position += pushdist;
            }
        }
    }
}
