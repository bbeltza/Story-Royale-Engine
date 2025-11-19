#pragma once
#include "Datatypes/Vector.hpp"
#include "Datatypes/Color.hpp"
#include "Datatypes/Rect.hpp"
#include "Base/File.hpp"

#define m_Texture ((SDL_Texture*)texture)
#define m_Surface ((SDL_Surface*)file_surface)

class Texture;

namespace Display
{
    void DrawTexture(Texture& _Texture, const RectF& Rectangle, const Color4& Modulate = Color4::WHITE, const Vector2f& AnchorPoint = Vector2f::CENTER);
}

extern "C" void __display_render();

class Texture
{
    typedef std::deque<Texture*> Queue;
    static Queue* to_load;
    static void load_textures();

    Texture() = default;
public:
    Texture(const Texture& other) = delete;
    Texture(void* from_surface);
    Texture(Texture&& moving) noexcept;
    Texture(const char* path);
    ~Texture();

    Vector2i GetSize();
private:
    void push_queue();

    void* texture = nullptr;
    // Temporary SDL_Surface* used to then load the texture to the GPU. Texture creating may be multithreaded, but creating textures may not be. So the image data is instead loaded into the CPU first.
    void* file_surface = nullptr;

    friend void Display::DrawTexture(Texture& _Texture, const RectF& Rectangle, const Color4& Modulate, const Vector2f& AnchorPoint);
    friend void __display_render();

    friend class TargetTexture;
    friend struct _containers_service;
};