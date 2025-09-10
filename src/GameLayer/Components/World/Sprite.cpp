#include "Engine.h"
#include "Game/Components/Sprite.h"

static Components::Sprite::Map& get_texture_files()
{
    static Components::Sprite::Map s_textures;
    return s_textures;
}

Components::Sprite::Sprite(): Scale(1, 1), Offset(0, 0)
{
    p_flags = p_Render;
}
Components::Sprite::~Sprite()
{
}

File& Components::Sprite::LoadFile(const char* path)
{
    auto& t_files = get_texture_files();
    std::string str_path = path; 

    if (t_files[str_path].getType() == File::T_UNKNOWN)
        t_files[str_path].Load(path);
    textures.emplace_back(&t_files.at(str_path));

    File* file = textures.back();
    return *file;
}