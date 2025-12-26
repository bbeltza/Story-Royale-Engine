#include <ECS/CameraControllers/entity.hpp>
#include <ECS/entity.hpp>
#include <utils/math.h>

#include <Engine.hpp>

using namespace sreECS;

void EntityControl::on_pupdate(Camera& camera)
{
    sre::vec2ut diff = m_target - m_entity->position;

    if (abs(diff.x) > bounds.x)
        m_target.x -= diff.x - bounds.x * ut_sign(diff.x);
    if (abs(diff.y) > bounds.y)
        m_target.y -= diff.y - bounds.y * ut_sign(diff.y);


    if (!smoothness)
        camera.position = m_target;
    else
        camera.position.setlerp(m_target, Runtime::delta_time / smoothness);
    
}