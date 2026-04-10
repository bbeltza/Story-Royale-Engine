#ifndef SREGUI_IMAGE_HPP
#define SREGUI_IMAGE_HPP

#include <GUI/Component.hpp>
#include <Base/Sampler.hpp>
#include <Datatypes/Color.hpp>

namespace sreGUI
{
    struct Transform;

    struct Image : public Component
    {
        sre::RSampler texture;
        sre::col4 modulate = sre::WHITE;

        void fit(Transform& transform);
    protected:
        void on_render(const sre::rect2Dut &dimensions) override;
    };
}

#endif