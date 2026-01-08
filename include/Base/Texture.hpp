#pragma once
#include "datatypes/vector.hpp"
#include "datatypes/color.hpp"
#include "datatypes/rect.hpp"
#include "Base/Image.hpp"

class Texture;

namespace sreECS
{
    struct Scene;
}

extern "C" void __display_render();

class Texture
{
    friend struct _containers_service;

    Texture(const Texture& other) = delete;
public:
    Texture() = default;
    Texture(const sre::Image& from_image);

    Texture(Texture&& moving) noexcept;
    ~Texture();

    sre::vec2i size() const;

    void* handle() const { return texture; }

protected: // Protected to allow custom behavior inherited texture classes
    void* texture = NULL;
};