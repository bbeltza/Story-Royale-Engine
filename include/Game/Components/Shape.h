#pragma once
#include "Game/Component.h"

#include "Datatypes/Rect.h"
#include "C/Color.h"

namespace Components
{
    class Shape : public ::Game::Component
    {
    public: // Enums
        enum ShapeForm : Flags8::Type
        {
            RectangleShape,
            CircleShape
        };
        enum Flags : Flags8::Type
        {
            VisibleFlag = (1 << 1),
            CanTouchFlag = (1 << 2),
            CanCollideFlag = (1 << 3),
            CanQueryFlag = (1 << 4),
        };

    public:
        Shape();

        // The rect representing the area of the shape by its size, and the offset from the entity by its position
        RectF Rect;
        Color4 Color = {255, 255, 255, 255};

        ShapeForm shape;
        Flags8 flags;

        void Render(::Game::Entity*) override;
        bool Query(::Game::Entity*) override;

        bool isInScreenPoint(::Game::Entity*, Vector2f);

        inline RectF getRealRect(::Game::Entity* parent) const;
        inline bool collidesWith(::Game::Entity* parent, const RectF& other_Rect) const {return getRealRect(parent).Intersects(other_Rect);}
    };
}