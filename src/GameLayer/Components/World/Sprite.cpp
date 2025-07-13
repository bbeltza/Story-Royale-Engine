#include "Engine.h"
#include "Components.h"

Game::Components::Sprite::Sprite(): Scale(1, 1), Offset(0, 0)
{
    p_flags = p_Render;
}
Game::Components::Sprite::~Sprite()
{
}

File& Game::Components::Sprite::LoadFile(const char* path)
{
    textures.emplace_back();

    File& file = textures.back();
    file.Load(path);
    return file;
}