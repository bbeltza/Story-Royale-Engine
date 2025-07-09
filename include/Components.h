#pragma once

#include "datatypes/Rect.h"
#include "File.h"
#include "ECS.h"

namespace Game
{
    namespace Components {

        class Shape : public Component
        {
        public:
            Shape();

            RectF Rect;
            Color4 Color = {255, 255, 255, 255};

            char shape;
            char flags;

            void render(int x, int y) override;

            bool isInScreenPoint(Vector2i pt);


            inline RectF getRealRect() const { auto parent = getParent(); return {parent->x + Rect.Position.X, parent->y + Rect.Position.Y, Rect.Size.X, Rect.Size.Y}; }
            inline bool collidesWith(const Shape* other_shape) const {return getRealRect().Intersects(other_shape->getRealRect());}

        private:
            friend class ::Game::Entity;
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

        class Sprite : public Component
        {
        public:
            Sprite();
            ~Sprite();

            Vector2f Offset;
            Vector2f Scale;

            size_t current_frame = 0;

            const char* spritePath = nullptr;

            File& LoadFile(const char* path);
            void render(int x, int y) override;
        private:
            std::vector<File> textures;
        };
    }
}