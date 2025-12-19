#pragma once
#include "datatypes/vector.hpp"
#include "datatypes/color.hpp"
#include "datatypes/rect.hpp"
#include "Base/Image.hpp"

class Texture;

namespace Game
{
    class World;
}

namespace Display
{
    void DrawTexture(const Texture& _Texture, sre::rect2Dut Rectangle, const sre::col4& Modulate, const sre::vec2f& AnchorPoint, const Game::World* world);
}

extern "C" void __display_render();

class Texture
{
    friend void Display::DrawTexture(const Texture& _Texture, sre::rect2Dut Rectangle, const sre::col4& Modulate, const sre::vec2f& AnchorPoint, const Game::World* world);
    friend struct _containers_service;

    Texture(const Texture& other) = delete;
public:
    Texture() = default;
    Texture(const sre::Image& from_image);

    Texture(Texture&& moving) noexcept;
    ~Texture();

    sre::vec2i size() const;
protected: // Protected to allow custom behavior inherited texture classes
    void* texture = NULL;
};