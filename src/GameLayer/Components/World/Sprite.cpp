#include "Engine.h"
#include "Game/Components/Sprite.h"

static std::unordered_map<std::string, File> *texture_files = nullptr;

const Components::Sprite::Initializer Components::Sprite::s_init;

Components::Sprite::Sprite(): Scale(1, 1), Offset(0, 0)
{
    if (!texture_files)
        texture_files = new std::unordered_map<std::string, File>;
    p_flags = p_Render;
}
Components::Sprite::~Sprite()
{
}

File& Components::Sprite::LoadFile(const char* path)
{
    auto& t_files = *texture_files;
    std::string str_path = path; 

    if (t_files[str_path].getType() == File::Type::Uninitialized)
        t_files[str_path].Load(path);
    textures.emplace_back(&texture_files->at(str_path));

    File* file = textures.back();
    return *file;
}

Components::Sprite::Initializer::~Initializer()
{
    if (texture_files)
        delete texture_files;
    texture_files = nullptr;
}