#pragma once
#include <ECS/Component.hpp>

#include <Datatypes/Rect.hpp>
#include <Datatypes/Color.hpp>

#include <Base/File.hpp>
#include <Core/Texture.hpp>

namespace sreECS
{
    class Sprite : public Component
    {       
    public:
        Sprite();
        ~Sprite();

        sre::vec2ut scale;
        sre::vec2ut offset;

        sre::col4 modulate = sre::col4::WHITE;
        sre::rect2Di region = { 0, 0 };

        sre::size_t current_frame = 0;

        void attach(sre::Texture&);
    private:
        void on_render(Entity&) override;

        std::vector<sre::Texture*> textures;

        void ImGuiUpdate();
    };
}