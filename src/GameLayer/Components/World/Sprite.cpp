#include "ECS.hpp"
#include "Game/Components/Sprite.hpp"

Components::Sprite::Sprite(): Scale(1, 1), Offset(0, 0) {}
Components::Sprite::~Sprite()
{
}

void Components::Sprite::Attach(Texture& texture)
{
    textures.push_back(&texture);
}

void Components::Sprite::Render(Game::Entity* entity)
{
    if (textures.empty()) return;

    auto frame = ut_min(current_frame, textures.size() - 1);
    current_frame = frame;
    Texture& texture = *textures[frame];

    RectF render_rect(
        entity->getWorld()->worldToScreenSpace(entity->Position.X, entity->Position.Y),
        texture.GetSize()
    );

    render_rect.Size *= Scale;
    render_rect.Position += Offset;

    Display::DrawTexture(texture, render_rect);
}
