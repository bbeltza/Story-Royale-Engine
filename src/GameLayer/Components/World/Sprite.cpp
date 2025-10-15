#include "Engine.hpp"
#include "Game/Components/Sprite.hpp"

Components::Sprite::Sprite(): Scale(1, 1), Offset(0, 0) {}
Components::Sprite::~Sprite()
{
}

void Components::Sprite::Attach(Texture& texture)
{
    textures.push_back(&texture);
}