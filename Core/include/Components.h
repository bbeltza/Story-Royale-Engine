#pragma once

#include "ECS.h"

namespace Game
{
    namespace Components {

        class Shape : public Component
        {
        public:
            Shape();
            int xOffset;
            int yOffset;
            int Width;
            int Height;

            Color4 Color = {255, 255, 255, 255};

            char shape;
            char flags;

            void render(int x, int y) override;
        private:
            friend void Game::Entity::pProcess(float dt);
            void processCollision();
            void processCollider();
        };

        class Velocity : public Component
        {
        public:
            Velocity();
            unsigned int velocity;
            float x_direction;
            float y_direction;

            void move(float dt);
            const float magnitude();

        private:
            void normalize();
        };
    }
}