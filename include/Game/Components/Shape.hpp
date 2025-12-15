#pragma once
#include "Game/Component.hpp"
#include "Game/Entity.hpp"

#include "datatypes/rect.hpp"
#include "datatypes/color.hpp"

namespace Components
{
    class Shape : public ::Game::Component
    {
        typedef std::unordered_set<Components::Shape *> CollisionSet;

    public: // Enums
        enum ShapeForm : sre::flags8::type
        {
            RectangleShape,
            CircleShape
        };
        enum Flags : sre::flags8::type
        {
            VisibleFlag = ut_bit(0),
            AnchoredFlag = ut_bit(1),
            CanTouchFlag = ut_bit(2),
            CanCollideFlag = ut_bit(3),
            CanQueryFlag = ut_bit(4),
        };

    public:
        static CollisionSet *collider_set;
        Shape();
        ~Shape();

        // The rect representing the area of the shape by its size, and the offset from the entity by its position
        sre::rect2Dut Rect;
        sre::col4 Color = {255, 255, 255, 255};

        ShapeForm shape;
        sre::flags8 flags;

        void pUpdate(::Game::Entity *, sre::timeStamp) override;
        void Render(::Game::Entity *) override;
        bool Query(::Game::Entity *, float*) override;

        bool isInScreenPoint(::Game::Entity *, sre::vec2ut);

        inline sre::rect2Dut getRealRect(::Game::Entity *parent) const { return {parent->Position + Rect.position, Rect.size}; }
        inline bool collidesWith(::Game::Entity *parent, const sre::rect2Dut &other_Rect) const { return getRealRect(parent).intersects(other_Rect); }
    };
}
