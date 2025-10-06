#pragma once
#include "Datatypes/Vector.hpp"
#include "Classes/File.hpp"

#define m_Texture ((SDL_Texture*)texture)
#define m_Surface ((SDL_Surface*)file_surface)

class Texture
{
    friend class DrawingDevice;
    friend class TargetTexture;
    typedef std::deque<Texture*> Queue;
    static Queue* to_load;

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
    // Temporary SDL_Surface* used to then load the texture to the GPU. Texture creating may be multithreaded, but creating textures cannot be. So the image data is instead loaded into the CPU first.
    void* file_surface = nullptr;
};