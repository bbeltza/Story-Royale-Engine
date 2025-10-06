#pragma once

#include "Classes/Texture.hpp"
#include "Datatypes/Rect.hpp"

// An object that when created, becomes the DrawingDevice's current target to render stuff on a separate texture.
// When one of these objects are present, render calls will now render stuff on the texture instead of the screen
// It can be stacked too with other target textures
// With Blit(), you render all the content to the last rendering target, or directly to the screen if there's no more TargetTextures behind
// But you can't directly render to the screen if there are already remaining TargetTextures
// You will also be able to create a Texture object with all you've rendered using your TargetTexture
class TargetTexture
{
    friend class DrawingDevice;
private:
    void* m_handle;
public:
    TargetTexture();
    ~TargetTexture();

    void Blit(const RectI& src, const RectF& dest, const Vector2f& anchor=Vector2f::CENTER);

    Texture CreateTexture(const RectI& src);

    void* operator new(size_t size) = delete;
};