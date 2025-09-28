#pragma once
#include "Datatypes/Vector.h"
#include "Classes/File.h"

#define m_Texture ((SDL_Texture*)texture)
#define m_Surface ((SDL_Surface*)file_surface)

class Texture
{
    friend class DrawingDevice;
    typedef std::deque<Texture*> Queue;

public:
    Texture(const Texture& other) = delete;
    Texture(void* from_texture);
    Texture(Texture&& moving);
    Texture(const char* path);
    Texture(int w, int h);
    ~Texture();

    Vector2i GetSize();
private:
    void push_queue();

    void* texture = nullptr;
    // Temporary SDL_Surface* used to then load the texture to the GPU. Texture creating may be multithreaded, but creating textures cannot be. So the image data is instead loaded into the CPU first.
    void* file_surface = nullptr;
};