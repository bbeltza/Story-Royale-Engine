#include "Engine.h"
#include "Game/Components/Sprite.h"

Components::Sprite::Sprite(): Scale(1, 1), Offset(0, 0)
{
    p_flags = p_Render;
}
Components::Sprite::~Sprite()
{
}

void Components::Sprite::Attach(Texture& texture)
{
    textures.push_back(&texture);
}