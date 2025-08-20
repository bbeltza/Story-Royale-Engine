#include <SDL.hpp>
#include "Game/Camera.h"

using namespace Game;

CameraController::CameraController()
{
    // Might be doing something in there
}

CameraController::~CameraController()
{
    if (m_Camera)
        m_Camera->m_Controller = nullptr;
}

void Camera::Update(TimeStamp dt)
{
    if (m_Controller && m_Controller->Enabled)
        m_Controller->Update(dt);

    if (clamp.Size)
    {
        x = SDL_clamp(x, clamp.getLeft(), clamp.getRight());
        y = SDL_clamp(y, clamp.getTop(), clamp.getBottom());
    }
}