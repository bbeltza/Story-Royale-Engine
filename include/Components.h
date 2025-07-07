#pragma once

#include <SDL.hpp>
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

            bool isInScreenPoint(Vector2i pt);

            // Get the current rect values, in world coordinates.
            inline SDL_FRect getRect();

            // Get the current rect values, in screen coordinates.
            inline SDL_FRect getScreenRect();
        private:
            friend class ::Game::Entity;
            
            SDL_FRect m_renderRect;
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

            float m_xNormalized, m_yNormalized;
        };
    }
}