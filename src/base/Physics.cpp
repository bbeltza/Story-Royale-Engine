#include <standard>

#include "Game/World.hpp"
#include "Game/Entity.hpp"
#include "Game/Component.hpp"
#include "Game/Components/Shape.hpp"

using namespace Components;

void Shape::pUpdate(::Game::Entity* entity, sre::timeStamp dt)
{    
    if (!collider_set) return;
    if (!flags.has(CanCollideFlag))
    {
        collider_set->erase(this);
        return;
    }
    collider_set->insert(this);

    if (flags.has(AnchoredFlag)) return;

    for (auto ent : entity->getWorld()->getEntities<Game::Entity>())
    {
        if (ent == entity) continue;

        for (auto comp : ent->getComponents<Shape>())
        {
            if (collider_set->count(comp) == 0) continue;

            sre::rect2Dut thisRect = getRealRect(entity),
                          otherRect = comp->getRealRect(ent);

            if (comp->collidesWith(ent, thisRect))
            {
                sre::vec2ut distance = (thisRect.position - otherRect.position);
                sre::vec2ut radius = (otherRect.size/2 + thisRect.size/2);

                sre::vec2ut sign = {distance.x < 0 ? -1.0_ut : 1, distance.y < 0 ? -1 : 1.0_ut};
                sre::vec2ut pushdist = radius * sign - distance;

                if (!comp->flags.has(AnchoredFlag)) pushdist = pushdist / 2;

                sre::unit absdx = abs(pushdist.x), absdy = abs(pushdist.y);

                if (absdx < absdy) entity->Position.x += pushdist.x;
                else if (absdx > absdy) entity->Position.y += pushdist.y;
                else entity->Position += pushdist;
            }
        }
    }
}
