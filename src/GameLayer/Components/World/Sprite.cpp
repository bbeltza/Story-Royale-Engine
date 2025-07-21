#include "Engine.h"
#include "Components.h"

static std::unordered_map<std::string, File> texture_files;

Game::Components::Sprite::Sprite(): Scale(1, 1), Offset(0, 0)
{
    //textures.reserve(16);
    p_flags = p_Render;
}
Game::Components::Sprite::~Sprite()
{
}

File& Game::Components::Sprite::LoadFile(const char* path)
{
    std::string str_path = path;
    if (texture_files[str_path].getType() == File::Type::Uninitialized)
        texture_files[str_path].Load(path);
    textures.emplace_back(&texture_files.at(str_path));

    File* file = textures.back();
    return *file;
}