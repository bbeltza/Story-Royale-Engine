#include <standard.h>
#include "ECS.h"

static std::vector<Game::Component*> CollisionStack;
static std::vector<Game::Component*> ColliderStack;

void Game::Entity::pProcess(float dt)
{
    if (m_Components.empty()) return;
    if (!velocityComp || velocityComp->VelocityComponent.x == 0 && velocityComp->VelocityComponent.y == 0);
    else
    {
        this->x += velocityComp->VelocityComponent.x * dt;
        this->y += velocityComp->VelocityComponent.y * dt;

        for (auto c : m_Components)
            c->procesCollider();
    }
    for (auto c : m_Components)
        c->procesCollision();


}

void Game::World::pUpdate(float dt)
{
    CollisionStack.clear();
    ColliderStack.clear();
    for (Entity* entity : m_Entities)
    {
        entity->pProcess(dt);
        entity->pUpdate(dt);
    }
    for (auto c1 : ColliderStack)
    {
        for (auto c2 : CollisionStack)
        {
            if (c1 == c2) continue;

            Entity* c1_ent = c1->getParent();
            Entity* c2_ent = c2->getParent();

            float c1x = c1_ent->x + c1->ShapeComponent.xOffset;
            float c1y = c1_ent->y + c1->ShapeComponent.yOffset;
            float c2x = c2_ent->x + c2->ShapeComponent.xOffset;
            float c2y = c2_ent->y + c2->ShapeComponent.yOffset;

            SDL_FRect r1{
                c1x,
                c1y,
                c1->ShapeComponent.Width,
                c1->ShapeComponent.Height
            };
            r1.x -= r1.w / 2;
            r1.y -= r1.h / 2;

            SDL_FRect r2{
                c2x,
                c2y,
                c2->ShapeComponent.Width,
                c2->ShapeComponent.Height
            };
            r2.x -= r2.w / 2;
            r2.y -= r2.h / 2;

            SDL_FRect res;
            if (!SDL_IntersectFRect(&r1, &r2, &res)) continue;

            //printf("res: { %f, %f, %f, %f }\n", res.x, res.y, res.h, res.w);
            switch (c1->ShapeComponent.Shape)
            {
            case CIRCLE:

                switch (c2->ShapeComponent.Shape)
                {


                case RECTANGLE:

                    break;
                default:
                    const float c1r = c1->ShapeComponent.Width / 2;
                    const float c2r = c2->ShapeComponent.Width / 2;
                    const float targetmg = c1r + c2r;

                    const float dstncx = c1x - c2x;
                    const float dstncy = c1y - c2y;
                    float mg = dstncx * dstncx + dstncy * dstncy;
                    mg = sqrt(mg);

                    if (mg <= targetmg)
                    {


                        c1_ent->x += dstncx / mg * targetmg - dstncx;
                        c1_ent->y += dstncy / mg * targetmg - dstncy;

                        //printf("%f, %f\n", , dstncy/mg*targetmg);
                    }

                    break;
                }
               break;
            default:
                if (res.h > res.w)
                {

                    c1_ent->x -= res.w * sign(r2.x - r1.x);
                }
                else if (res.h < res.w)
                {
                    c1_ent->y -= res.h * sign(r2.y - r1.y);
                }
               break;
            }

        }
    }
}

void Game::Component::procesCollision()
{
    if (this->m_type == SHAPE)
    {
        if (ShapeComponent.Flags & (CAN_COLLIDE | CAN_TOUCH))
        {
            CollisionStack.push_back(this);
            //std::cout << this << "\n";
        }
    }
}

void Game::Component::procesCollider()
{
    if (this->m_type == SHAPE)
    {
        if (ShapeComponent.Flags & (CAN_COLLIDE | CAN_TOUCH))
        {
            ColliderStack.push_back(this);
            //std::cout << this << "\n";
        }
    }
}