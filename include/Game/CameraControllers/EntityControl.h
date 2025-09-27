#pragma once
#include "Game/CameraControllers/API.h"
#include "Game/Entity.h"

namespace CameraControllers
{
    cam_controldef(EntityControl)
    {
    public:
        EntityControl(Game::Entity& entity): m_Entity(&entity) {}
        ~EntityControl();

        float Smoothness = 0.1f;

    protected:
        void Update(TimeStamp) override;

    private:
        Game::Entity *const m_Entity;
    };
}