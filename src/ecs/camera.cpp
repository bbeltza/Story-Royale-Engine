#include <ECS/scene.hpp>
#include <ECS/camera.hpp>

#include <Base/Display.hpp>

using namespace sreECS;

void Camera::update()
{
    if (controller && controller->enabled)
        controller->on_update(*this);
}

void Camera::pupdate()
{
    if (controller && controller->enabled)
        controller->on_pupdate(*this);
    
    if (bounds.size != bounds.size.ZERO) { position.setclamp(bounds.top_left(), bounds.bottom_right()); }

    if (effect && effect->enabled)
        position += effect->on_process();
    
    const sre::unit scale = ::Display::GetScale();

    m_rpos.x = floor(position.x * scale) / scale;
    m_rpos.y = floor(position.y * scale) / scale;
}

static const Camera ZERO_CAM;

sre::vec2ut Camera::toWorldSpace(sre::vec2ut coords) const
{
    auto _this = this;
    if (_this <= &reinterpret_cast<Scene*>(0)->camera)
        _this = &ZERO_CAM;

    return coords - Display::GetCenter() + _this->m_rpos;
}

sre::vec2ut Camera::toScreenSpace(sre::vec2ut coords) const
{
    auto _this = this;
    if (_this <= &reinterpret_cast<Scene*>(0)->camera)
        _this = &ZERO_CAM;

    const sre::unit scale = Display::GetScale();

    sre::vec2ut pos = coords + Display::GetCenter() - _this->m_rpos;
    pos.x = floor(pos.x * scale);
    pos.y = floor(pos.y * scale);

    return pos;
}
