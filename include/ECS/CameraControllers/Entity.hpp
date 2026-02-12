#pragma once
#include <ECS/Camera.hpp>

namespace sreECS
{
    class Entity;

    class EntityControl: public CameraController
    {
    public:
        EntityControl(Entity& entity): m_entity(&entity) {}

        sre::unit smoothness = 0.10_ut;
        sre::vec2ut bounds;

    protected:
        void on_pupdate(Camera& camera) override;
    private:
        Entity* const m_entity;
        sre::vec2ut m_target;
    };
}