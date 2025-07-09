#include <standard.h>
#include <SDL.hpp>

#include "ECS.h"
#include "Components.h"

const double pi = M_PI;
static std::vector<Game::Components::Shape*> CollisionStack;
static std::vector<Game::Components::Shape*> ColliderStack;

static void processCollider(Game::Components::Shape* shape)
{
    if (shape->flags & (CAN_COLLIDE | CAN_TOUCH))
    {
            ColliderStack.push_back(shape);
            //std::cout << this << "\n";
    }
}
static void processCollision(Game::Components::Shape* shape)
{
    if (shape->flags & (CAN_COLLIDE | CAN_TOUCH))
    {
            CollisionStack.push_back(shape);
            //std::cout << this << "\n";
    }
}

void Game::Entity::_pProcess(float dt)
{
    if (m_Components.empty()) return;

    Components::Velocity* vComp = (Components::Velocity*)velocityComp;
    if (!velocityComp || vComp->x_direction == 0 && vComp->y_direction == 0);
    else
    {
        vComp->move(dt);

        for (auto c : m_Components)
        {
            if (c->getType() == SHAPE)
                processCollider((Components::Shape*)c);
                
        }
            
    }
    for (auto c : m_Components)
    {
        if (c->getType() == SHAPE)
            processCollision((Components::Shape*)c);
    }
        


}

static float c1x, c1y, c2x, c2y;
static Game::Entity* c1_ent, *c2_ent;

static void collideCircle(Game::Components::Shape* comp, float x, float y, float r)
{
    const float c1r = comp->Rect.Size.X / 2;
    const float targetmg = c1r + r;

    const float dstncx = c1x - x;
    const float dstncy = c1y - y;
    float mg = dstncx * dstncx + dstncy * dstncy;
    mg = sqrt(mg);

    if (mg <= targetmg)
    {
        c1_ent->x += dstncx / mg * targetmg - dstncx;
        c1_ent->y += dstncy / mg * targetmg - dstncy;
    }
}

static inline bool collideRectX(const SDL_FRect& res)
{
    if (res.h > res.w)
    {
        c1_ent->x -= res.w * sign(c2x - c1x);
        return 1;
    }
    return 0;
}

static inline void collideRectY(const SDL_FRect& res)
{
    if (res.h < res.w)
    {
        c1_ent->y -= res.h * sign(c2y - c1y);
    }
}

static inline void collideCircleRect(Game::Components::Shape* comp, const SDL_FRect& res, const SDL_FRect& r1, const SDL_FRect& r2)
{
    if (abs(c2y - c1y) < r2.h / 2) collideRectX(res);
    else if (abs(c2x - c1x) < r2.w / 2) collideRectY(res);
    else
    {
        collideCircle(comp,
            c2x + r2.w / 2 * sign(r1.x - r2.x),
            c2y + r2.w / 2 * sign(r1.y - r2.y),
            0
        );
    }
}


void Game::World::pUpdate(float dt)
{
    CollisionStack.clear();
    ColliderStack.clear();
    for (Entity* entity : m_Entities)
    {
        entity->_pProcess(dt);
        entity->pUpdate(dt);
    }
    while (!ColliderStack.empty())
    {
        Components::Shape* c1 = ColliderStack.back();
        ColliderStack.pop_back();
        for (auto c2 : CollisionStack)
        {
            if (c1 == c2) continue;

            c1_ent = c1->getParent();
            c2_ent = c2->getParent();

            c1x = c1_ent->x + c1->Rect.Position.X;
            c1y = c1_ent->y + c1->Rect.Position.Y;
            c2x = c2_ent->x + c2->Rect.Position.X;
            c2y = c2_ent->y + c2->Rect.Position.Y;

            SDL_FRect r1{
                c1x,
                c1y,
                c1->Rect.Size.X,
                c1->Rect.Size.Y
            };
            r1.x -= r1.w / 2;
            r1.y -= r1.h / 2;

            SDL_FRect r2{
                c2x,
                c2y,
                c2->Rect.Size.X,
                c2->Rect.Size.Y
            };
            r2.x -= r2.w / 2;
            r2.y -= r2.h / 2;

            SDL_FRect res;
            if (!SDL_IntersectFRect(&r1, &r2, &res)) continue;

            //printf("res: { %f, %f, %f, %f }\n", res.x, res.y, res.h, res.w);

            if (c1->shape == CIRCLE && c2->shape == CIRCLE)
                collideCircle(c1, c2x, c2y, c2->Rect.Size.X / 2);
            else if (c1->shape == RECTANGLE && c2->shape == RECTANGLE)
            {
                if (!collideRectX(res)) collideRectY(res);
            }
            else
                collideCircleRect(c1, res, r1, r2);

        }
    }
    c1_ent = nullptr;
    c2_ent = nullptr;

}