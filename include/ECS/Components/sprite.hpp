#pragma once
#include <ECS/component.hpp>

#include <datatypes/vector.hpp>
#include <datatypes/color.hpp>

#include <Base/File.hpp>
#include <Base/Texture.hpp>

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

        size_t current_frame = 0;

        
        void attach(Texture&);
    private:
        void on_render(Entity&) override;

        std::vector<Texture*> textures;
    };
}