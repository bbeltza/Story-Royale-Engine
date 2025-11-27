#pragma once
#include "Game/CameraControllers/API"
#include "Game/Entity.hpp"

namespace CameraControllers
{
    cam_controldef(EntityControl)
    {
    public:
        EntityControl(Game::Entity* entity): m_Entity(entity) {}
        ~EntityControl();

        float Smoothness = 0.13f;
        Vector2ut Bounds;

    protected:
        void pUpdate(TimeStamp) override;

    private:
        Game::Entity *const m_Entity;
        Vector2ut m_Target;
    };
}