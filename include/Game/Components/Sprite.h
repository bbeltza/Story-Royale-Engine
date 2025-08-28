#pragma once
#include "Game/Component.h"

#include "Datatypes/Vector.h"

#include "Classes/File.h"

namespace Components
{
    class Sprite : public ::Game::Component
    {       
    public:
        typedef std::unordered_map<std::string, File> Map;

        Sprite();
        ~Sprite();

        Vector2f Offset;
        Vector2f Scale;

        size_t current_frame = 0;

        void Render(::Game::Entity*) override;

        File& LoadFile(const char*);
    private:
        std::vector<File*> textures;
    };
}