#pragma once
#include <ECS/Component.hpp>
#include <ECS/Entity.hpp>

#include <Datatypes/Rect.h>
#include <Datatypes/Color.h>
#include <Datatypes/Flags.hpp>

namespace sreECS
{
    class Shape : public Component
    {
    public: // Enums
        enum ShapeForm
        {
            S_RECTANGLE,
            S_CIRCLE
        };
        enum Flags
        {
            F_RESERVED = F_ENABLED,
            F_VISIBLE = ut_bit(1),
            F_ANCHORED = ut_bit(2),
            F_CANCOLLIDE = ut_bit(3),
            F_CANQUERY = ut_bit(4)
        };

    public:
        Shape(
            sre::flagsptr flags=F_VISIBLE,
            sre::rect2Dut rect={},
            sre::col4 color=sre::WHITE
        );
        ~Shape();

        // The rect representing the area of the shape by its size, and the offset from the entity by its position
        sre::rect2Dut rect{ 0, 100 };
        sre::col4 color = sre::WHITE;

        ShapeForm shape = S_RECTANGLE;

        void on_render(Entity&) override;
        void on_pupdate(Entity&, sre::timeStamp) override;
        bool on_query(Entity&, sre::vec2ut) const override;

        bool in_screen_point(Entity& entity, sre::vec2ut) const;

        inline sre::rect2Dut real_rect(Entity& entity) const { return {entity.position + rect.position, rect.size}; }
        inline bool collides(Entity& entity, const sre::rect2Dut &other_Rect) const { return real_rect(entity).intersects_from_origin(other_Rect, sre::vec2ut{-0.5_ut}); }
    };
}