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

            void Render(Entity* _entity) override;
            //void pUpdate(Entity* _entity, float delta) override;
            bool Query(Entity* _entity) override;

            bool isInScreenPoint(Entity*, Vector2i);


            inline RectF getRealRect(Entity* parent) const { return {parent->x + Rect.Position.X, parent->y + Rect.Position.Y, Rect.Size.X, Rect.Size.Y}; }
            inline bool collidesWith(Entity* parent, const RectF& other_Rect) const {return getRealRect(parent).Intersects(other_Rect);}

        private:
            friend class ::Game::Entity;
        };

        class Velocity : public Component
        {
            friend Velocity* Entity::addVelocityComponent();
            Velocity(); // To assure it doesn't get constructed by addComponent()
           public:
            unsigned int velocity;
            float x_direction;
            float y_direction;

            void move(Entity* e, float dt);
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
            void Render(Entity* _entity) override;
        private:
            std::vector<File*> textures;
        };
    }
}