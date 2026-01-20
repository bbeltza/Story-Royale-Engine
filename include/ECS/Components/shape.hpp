#pragma once
#include <ECS/component.hpp>
#include <ECS/entity.hpp>

#include <Datatypes/Rect.hpp>
#include <Datatypes/Color.hpp>
#include <Datatypes/Flags.hpp>

namespace sreECS
{
    class Shape : public Component
    {
    public: // Enums
        enum ShapeForm : sre::flags8::type
        {
            S_RECTANGLE,
            S_CIRCLE
        };
        enum Flags : sre::flags8::type
        {
            F_VISIBLE = ut_bit(0),
            F_ANCHORED = ut_bit(1),
            F_CANCOLLIDE = ut_bit(2),
            F_CANQUERY = ut_bit(3),
        };

    public:
        Shape();
        ~Shape();

        // The rect representing the area of the shape by its size, and the offset from the entity by its position
        sre::rect2Dut rect;
        sre::col4 color = sre::col4::WHITE;

        ShapeForm shape;
        sre::flags8 flags;

        void on_pupdate(Entity&) override;
        void on_render(Entity&) override;
        bool on_query(Entity&, sre::vec2ut) const override;

        bool in_screen_point(Entity& entity, sre::vec2ut) const;

        inline sre::rect2Dut real_rect(Entity& entity) const { return {entity.position + rect.position, rect.size}; }
        inline bool collides(Entity& entity, const sre::rect2Dut &other_Rect) const { return real_rect(entity).intersects(other_Rect); }
    };
}