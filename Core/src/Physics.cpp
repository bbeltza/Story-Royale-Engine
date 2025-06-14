#include <standard.h>
#include "ECS.h"

static std::vector<Game::Component*> CollisionStack;

void Game::Entity::pProcess(float dt)
{
    if (m_Components.empty()) return;

    for (auto comp : m_Components)
    {
        comp->pProcess(dt);
    }
}

void Game::World::pUpdate(float dt)
{
    CollisionStack.clear();
    for (Entity* entity : m_Entities)
    {
        entity->pProcess(dt);
        entity->pUpdate(dt);
    }
    for (auto c1 : CollisionStack)
    {
        for (auto c2 : CollisionStack)
        {
            if (c1 == c2) continue;
            auto f = std::find(c2->collided.begin(), c2->collided.end(), c1);
            if (f != c2->collided.end()) continue;

            Entity* c1_ent = c1->getParent();
            Entity* c2_ent = c2->getParent();

            SDL_FRect r1{
                c1_ent->x + c1->ShapeComponent.xOffset,
                c1_ent->y + c1->ShapeComponent.yOffset,
                c1->ShapeComponent.Width,
                c1->ShapeComponent.Height
            };
            r1.x -= r1.w / 2;
            r1.y -= r1.h / 2;

            SDL_FRect r2{
                c2_ent->x + c2->ShapeComponent.xOffset,
                c2_ent->y + c2->ShapeComponent.yOffset,
                c2->ShapeComponent.Width,
                c2->ShapeComponent.Height
            };
            r2.x -= r2.w / 2;
            r2.y -= r2.h / 2;

            SDL_FRect res;
            if (!SDL_IntersectFRect(&r1, &r2, &res)) continue;

            //printf("res: { %f, %f, %f, %f }\n", res.x, res.y, res.h, res.w);
            if (res.h > res.w)
            {
                
                c1_ent->x -= res.w * sign(r2.x - r1.x);
            }
            else if (res.h < res.w)
            {
                c1_ent->y -= res.h * sign(r2.y - r1.y);
            }

            c1->collided.push_back(c2);
        }
    }
}

void Game::Component::pProcess(float dt)
{
    if (this->m_type == SHAPE)
    {
        if (ShapeComponent.Flags & (CAN_COLLIDE | CAN_TOUCH))
        {
            collided.clear();
            CollisionStack.push_back(this);
        }
    }
}