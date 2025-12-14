#include "Game/CameraControllers/EntityControl.hpp"
#include "utils/math.h"

use_namespace

EntityControl::~EntityControl()
{

}

void EntityControl::pUpdate(sre::timeStamp dt)
{
    Camera& CurrentCamera = *getCamera();

    sre::vec2ut diff = m_Target - m_Entity->Position;

    if (abs(diff.x) > Bounds.x)
        m_Target.x -= diff.x - Bounds.x * ut_sign(diff.x);
    if (abs(diff.y) > Bounds.y)
        m_Target.y -= diff.y - Bounds.y * ut_sign(diff.y);


    if (!Smoothness)
        CurrentCamera.position = m_Target;
    else
        CurrentCamera.position.setlerp(m_Target, dt / Smoothness);
    
}