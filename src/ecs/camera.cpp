#include <ECS/Scene.hpp>
#include <ECS/Camera.hpp>

#include <Core/Window.hpp>
#include <Core/Render.h>

using namespace sreECS;

void Camera::update()
{
    if (controller && controller->enabled)
        controller->on_update(*this);
}

void Camera::pupdate(sre::timeStamp dt)
{
    if (dt && controller && controller->enabled)
        controller->on_pupdate(*this, dt);
}

sre::vec2ut Camera::toWorldSpace(sre::vec2ut coords) const
{
    const auto _this = this;
    if (_this > &reinterpret_cast<Scene*>(0)->camera)
        coords += position;

    // This will only take the main global layer's viewport, better ways of parenting a camera to a scene aren't implemented yet!
    return coords - sre::calc_viewport_size(sreECS::get_viewport_area(), sreECS::get_viewport_scale())*0.5_ut;
}

sre::vec2ut Camera::toScreenSpace(sre::vec2ut coords) const
{
    auto _this = this;
    if (_this > &reinterpret_cast<Scene*>(0)->camera)
        coords -= position;
    
    return coords + sre::calc_viewport_size(sreECS::get_viewport_area(), sreECS::get_viewport_scale())*0.5_ut;
}
