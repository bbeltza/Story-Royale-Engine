#pragma once
#include "Game/CameraControllers/API.h"
#include "Game/Entity.h"

namespace CameraControllers
{
    cam_controldef(EntityControl)
    {
    public:
        EntityControl(Game::Entity &);
        ~EntityControl();

    protected:
        void Update(TimeStamp) override;

    private:
        Game::Entity *const m_Entity;
    };
}