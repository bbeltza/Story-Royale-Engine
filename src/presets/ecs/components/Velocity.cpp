#include <ECS/Entity.hpp>
#include <ECS/Components/Velocity.hpp>

#include <Core/Runtime.hpp>

using namespace sreECS;

void Velocity::normalize(sre::unit speed)
{
    if (speed == 0)
    {
        velocity = velocity.ZERO;
        return;
    }
    
    sre::unit mag = static_cast<sre::unit>(velocity.magnitude());
    if (mag == 0.0) return;

    velocity = velocity / mag * speed;
}

void Velocity::on_pupdate(Entity& e)
{
    e.position += velocity * sre::pdt;
}

#ifndef IMGUI_DISABLE
    void Velocity::ImGuiUpdate()
    {
        ImGui::DragFloat2("velocity", velocity);
    }
#endif