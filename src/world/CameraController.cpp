#include "utils/math.h"
#include "Game/Camera.hpp"
#include "Base/Display.hpp"

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
}

void Camera::pUpdate(TimeStamp dt)
{
    if (m_Controller && m_Controller->Enabled)
        m_Controller->pUpdate(dt);

    if (clamp.size != clamp.size.ZERO) { position.setclamp(clamp.top_left(), clamp.bottom_right()); }

    const sre::unit scale = ::Display::GetScale();

    rpos.x = floor(position.x * scale) / scale;
    rpos.y = floor(position.y * scale) / scale;
}