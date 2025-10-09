#include <standard>

#include "Engine.hpp"
#include "Game/Camera.hpp"
#include "Game/World.hpp"
#include "Game/GuiLayer.hpp"
#include "Game/GuiComponent.hpp"

#include "Game/GuiComponents/Text.hpp"

#include "GameSettings.hpp"
#include "Sys.h"

#include "config.h"

Vector2f DrawingDevice::getScreenCenter()
{
    return {m_viewport.w / 2.0f, m_viewport.h / 2.0f};
}

#define START_DRAW SDL_LockMutex(m_lockmutex);
#define END_DRAW SDL_UnlockMutex(m_lockmutex);

void DrawingDevice::DrawRectangle(const RectF& Rectangle, const Color4& Color, const Color4& Modulate, const Vector2f &AnchorPoint, DrawingMode Mode)
{
    START_DRAW

    SDL_FRect r{
        Rectangle.Position.X - Rectangle.Size.X * AnchorPoint.X,
        Rectangle.Position.Y - Rectangle.Size.Y * AnchorPoint.Y,
        Rectangle.Size.X,
        Rectangle.Size.Y};

    SDL_SetRenderDrawColorMod(sdl_renderer, (SDL_Color*)&Color, (SDL_Color*)&Modulate);
    switch (Mode)
    {
    case dm_Stroke:
        SDL_RenderDrawRectF(sdl_renderer, &r);
        break;
    
    default:
        SDL_RenderFillRectF(sdl_renderer, &r);
        break;
    }

    END_DRAW
};

void DrawingDevice::DrawRectangleAtWorld(RectF Rectangle, const Color4& Color, const Color4& Modulate, const Vector2f &AnchorPoint, DrawingMode Mode)
{
    START_DRAW

    Rectangle.Position = Game::World::worldToScreen(Rectangle.Position.X, Rectangle.Position.Y, Game::World::currentCamera());
    DrawRectangle(Rectangle, Color, Modulate, AnchorPoint, Mode);

    END_DRAW
}

void DrawingDevice::DrawRotatedRectangle(const RectF &_Rectangle, const double _angle, const Color4 &_Col, DrawingMode _dm)
{
    START_DRAW

    if (_angle == 0)
        return DrawRectangle(_Rectangle, _Col);

    switch (_dm)
    {
    case dm_Stroke:
        {
            Vector2f points[4] = 
            {
                _Rectangle.getTopLeftRotated(_angle),
                _Rectangle.getTopRightRotated(_angle),
                _Rectangle.getBottomRightRotated(_angle),
                _Rectangle.getBottomLeftRotated(_angle)
            };
            SDL_RenderDrawLinesF(sdl_renderer, (SDL_FPoint*)points, 4);
        }
        break;
    default:
        {
            SDL_FRect r{
            _Rectangle.getLeft(),
            _Rectangle.getTop(),
            _Rectangle.Size.X,
            _Rectangle.Size.Y};

            SDL_SetTextureColorMod(sdl_rectTexture, _Col.r, _Col.g, _Col.b);
            SDL_SetTextureAlphaMod(sdl_rectTexture, _Col.a);
            SDL_RenderCopyExF(sdl_renderer, sdl_rectTexture, NULL, &r, _angle, NULL, SDL_FLIP_NONE);
        }
        break;
    }

    END_DRAW
}

void DrawingDevice::DrawRotatedRectangleAtWorld(const RectF &_Rectangle, const double _angle, const Color4 &_Col, DrawingMode _dm)
{
    START_DRAW

    Vector2f target_pos(_Rectangle.Position);
    if (Game::World::m_Current)
        target_pos = Game::World::m_Current->worldToScreenSpace(target_pos.X, target_pos.Y);
    else
        target_pos = Game::World::worldToScreen(target_pos.X, target_pos.Y);

    DrawRotatedRectangle(RectF(target_pos.X, target_pos.Y, _Rectangle.Size.X, _Rectangle.Size.Y), _angle, _Col, _dm);

    END_DRAW
}

void DrawingDevice::DrawDebug(Vector2f pos) // Sounds weird to not pass as a reference, but it will allow us to get a copy to convert it into screen coordinates
{
    START_DRAW

    if (Game::World::m_Current)
        pos = Game::World::m_Current->worldToScreenSpace(pos.X, pos.Y);
    else
        pos = Game::World::worldToScreen(pos.X, pos.Y);

    SDL_SetRenderDrawColor(sdl_renderer, 255, 64, 0, 255);
    SDL_RenderDrawLineF(sdl_renderer, pos.X - DRAW_ENTCENTER_LINESIZE, pos.Y, pos.X + DRAW_ENTCENTER_LINESIZE, pos.Y);
    SDL_RenderDrawLineF(sdl_renderer, pos.X, pos.Y - DRAW_ENTCENTER_LINESIZE, pos.X, pos.Y + DRAW_ENTCENTER_LINESIZE);

    END_DRAW
}

void DrawingDevice::DrawCircle(const Vector2f& _Pos, const float _Radius, const Color4& _Col, DrawingMode _dm)
{
    START_DRAW

    SDL_RenderFillCircleF(sdl_renderer, _Pos.X, _Pos.Y, _Radius);

    END_DRAW
}

void DrawingDevice::renderCurrentWorld()
{
    auto world = Game::World::m_Current;

    //// Aliases for the background and the foreground (so that typing Game::currentWorld wouldn't be necessary)
    Color3 &bg = world->Background;
    Color4 &fg = world->Foreground;

    //// Clearing the screen with the background color
    SDL_SetRenderDrawColor(sdl_renderer, bg.r, bg.g, bg.b, 255);
    SDL_RenderClear(sdl_renderer);

    //// Drawing all the entities (doesn't run if the foreground is full opaque)
    if (fg.a < 255)
    {
        world->call_render();
    }

    //// Finally, filling the foreground (doesn't run if the foreground is invisible)
    // printf("%u, %u, %u, %u / %u, %u, %u\n", fg.r, fg.g, fg.b, fg.a, bg.r, bg.g, bg.b);
    if (fg.a)
    {
        SDL_SetRenderDrawColor(sdl_renderer, fg.r, fg.g, fg.b, fg.a);
        SDL_RenderFillRect(sdl_renderer, NULL);
    }
}

void DrawingDevice::renderCurrentUI()
{
    auto layer = Game::GuiLayer::Current();

    Color4 &uifg = layer->Foreground;

    if (uifg.a < 255)
    {
        layer->preRender();
        layer->_renderchildren();
        layer->postRender();
    }
    if (uifg.a)
    {
        SDL_SetRenderDrawColor(sdl_renderer, uifg.r, uifg.g, uifg.b, uifg.a);
        SDL_RenderFillRect(sdl_renderer, NULL);
    }
}

void DrawingDevice::DrawTexture(Texture& _Texture, const RectF& Rectangle, const Color4& Modulate, const Vector2f& AnchorPoint)
{
    START_DRAW

    if (Rectangle.Size.X == 0 || Rectangle.Size.Y == 0)
        return;
    
    if (!_Texture.texture) load_textures();

    float absW = abs(Rectangle.Size.X);
    float absH = abs(Rectangle.Size.Y);

    float rl = Rectangle.Position.X - absW * AnchorPoint.X;
    float rt = Rectangle.Position.Y - absH * AnchorPoint.Y;

    float left = roundf(rl * scale) / scale;
    float top = roundf(rt * scale) / scale;
    SDL_FRect render_rect{left, top, absW, absH};
    int flip = (Rectangle.Size.X < 0 ? ut_bit(0) : 0) | (Rectangle.Size.Y < 0 ? ut_bit(1) : 0);

    SDL_SetTextureColorMod((SDL_Texture*)_Texture.texture, Modulate.r, Modulate.g, Modulate.b);
    SDL_SetTextureAlphaMod((SDL_Texture*)_Texture.texture, Modulate.a);
    SDL_RenderCopyExF(sdl_renderer, (SDL_Texture *)_Texture.texture, NULL, &render_rect, 0, NULL, (SDL_RendererFlip)flip);

    #if 0
    SDL_SetRenderDrawColor(sdl_renderer, 255, 0, 0, 255);
    SDL_RenderDrawRectF(sdl_renderer, &render_rect);
    #endif

    END_DRAW
}

//

//

static Game::Camera zeroCam;

Vector2f Game::World::screenToWorld(float x, float y, Camera *cam)
{
    if (!cam)
        return screenToWorld(x, y, &zeroCam);
    return Vector2f(x - center.X + cam->x, y - center.Y + cam->y);
}

Vector2f Game::World::worldToScreen(float x, float y, Camera *cam)
{
    if (!cam)
        return worldToScreen(x, y, &zeroCam);
    return Vector2f(x + center.X - cam->x, y + center.Y - cam->y);
}

// LEGACY

bool DrawingDevice::LegacyLoadFileTexture(File &_File)
{
    // printf("%p\n", _File.m_userdata);
    if (_File.m_userdata)
        return true;

    if (SDL_Texture *target_texture = m_LoadedTextures[_File.m_filepath])
    {
        _File.m_userdata = target_texture;
        return true;
    }
    else
    {
        SDL_RWops *temp_rw = SDL_RWFromConstMem(_File.getRawData(), (int)_File.getSize());
        if (!temp_rw)
            goto err;

        if (IMG_isPNG(temp_rw))
            _File.m_type = File::T_PNG;

        m_LoadedTextures[_File.m_filepath] = IMG_LoadTexture_RW(sdl_renderer, temp_rw, 1);
        if (!m_LoadedTextures[_File.m_filepath])
            goto err;
        _File.m_userdata = m_LoadedTextures[_File.m_filepath];
        return true;
    }

err:
    printf("%s\n", SDL_GetError());
    return false;
}

void DrawingDevice::LegacyDrawTexture(const RectF& _Rectangle, File &_File)
{
    if (!LegacyLoadFileTexture(_File))
        return;
    if (_Rectangle.Size.X == 0 || _Rectangle.Size.Y == 0)
        return;

    START_DRAW
    
    float left = roundf(_Rectangle.getLeft() * scale) / scale;
    float top = roundf(_Rectangle.getTop() * scale) / scale;
    SDL_FRect render_rect{left, top, abs(_Rectangle.Size.X), abs(_Rectangle.Size.Y)};
    int flip = (_Rectangle.Size.X < 0 ? ut_bit(0) : 0) | (_Rectangle.Size.Y < 0 ? ut_bit(1) : 0);

    SDL_RenderCopyExF(sdl_renderer, (SDL_Texture *)_File.m_userdata, NULL, &render_rect, 0, NULL, (SDL_RendererFlip)flip);

    END_DRAW
}
