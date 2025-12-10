#pragma once
#include "Datatypes/Vector.hpp"
#include "Datatypes/Color.hpp"
#include "Datatypes/Rect.hpp"
#include "Base/Image.hpp"

class Texture;

namespace Game
{
    class World;
}

namespace Display
{
    void DrawTexture(const Texture& _Texture, RectUt Rectangle, const Color4& Modulate, const Vector2f& AnchorPoint, const Game::World* world);
}

extern "C" void __display_render();

class Texture
{
    friend void Display::DrawTexture(const Texture& _Texture, RectUt Rectangle, const Color4& Modulate, const Vector2f& AnchorPoint, const Game::World* world);
    friend struct _containers_service;

    Texture(const Texture& other) = delete;
public:
    Texture() = default;
    Texture(const sre::Image& from_image);

    Texture(Texture&& moving) noexcept;
    ~Texture();

    Vector2i size() const;
protected: // Protected to allow custom behavior inherited texture classes
    void* texture = NULL;
};