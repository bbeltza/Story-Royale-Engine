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

    if (!clamp.Size.isZero())
    {
        x = ut_clamp(x, clamp.getLeft(), clamp.getRight());
        y = ut_clamp(y, clamp.getTop(), clamp.getBottom());
    }

    const Unit scale = ::Display::GetScale();

    rx = floor(x * scale) / scale;
    ry = floor(y * scale) / scale;
}