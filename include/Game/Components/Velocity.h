#pragma once
#include "Game/Component.h"

#include "Datatypes/Vector.h"

namespace Components
{
    class Velocity: public ::Game::Component
    {
        public:
        Velocity() { p_flags = p_pUpdate; }
        Vector2f velocity;

        void pUpdate(::Game::Entity*, delta_model) override;
    };
}