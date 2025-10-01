#pragma once
#include "Game/Component.hpp"
#include "Game/Entity.hpp"

#include "Datatypes/Rect.hpp"
#include "Datatypes/Color.hpp"

namespace Components
{
    class Shape : public ::Game::Component
    {
        typedef std::unordered_set<Components::Shape *> CollisionSet;

    public: // Enums
        enum ShapeForm : Flags8::Type
        {
            RectangleShape,
            CircleShape
        };
        enum Flags : Flags8::Type
        {
            VisibleFlag = bit(0),
            AnchoredFlag = bit(1),
            CanTouchFlag = bit(2),
            CanCollideFlag = bit(3),
            CanQueryFlag = bit(4),
        };

    public:
        static CollisionSet *collider_set;
        Shape();
        ~Shape();

        // The rect representing the area of the shape by its size, and the offset from the entity by its position
        RectF Rect;
        Color4 Color = {255, 255, 255, 255};

        ShapeForm shape;
        Flags8 flags;

        void pUpdate(::Game::Entity *, TimeStamp) override;
        void Render(::Game::Entity *) override;
        bool Query(::Game::Entity *) override;

        bool isInScreenPoint(::Game::Entity *, Vector2f);

        inline RectF getRealRect(::Game::Entity *parent) const { return {parent->Position + Rect.Position, Rect.Size}; }
        inline bool collidesWith(::Game::Entity *parent, const RectF &other_Rect) const { return getRealRect(parent).Intersects(other_Rect); }
    };
}
