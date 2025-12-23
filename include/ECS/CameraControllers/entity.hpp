#pragma once
#include <ECS/camera.hpp>

namespace sreECS
{
    class Entity;

    class EntityControl: public CameraController
    {
    public:
        EntityControl(Entity& entity): m_entity(&entity) {}

        float smoothness = 0.13f;
        sre::vec2ut bounds;

    protected:
        void on_pupdate(Camera& camera) override;
    private:
        Entity* const m_entity;
        sre::vec2ut m_target;
    };
}